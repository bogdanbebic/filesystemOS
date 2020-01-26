#include "KernelFS.h"
#include "DirDataCluster.h"
#include "File.h"
#include "KernelFile.h"

// ReSharper disable once CommentTypo
KernelFS::KernelFS()  // NOLINT(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
{
	InitializeCriticalSection(&this->mount_critical_section_);
	InitializeConditionVariable(&this->mount_cv_);

	InitializeCriticalSection(&this->unmount_critical_section_);
	InitializeConditionVariable(&this->unmount_cv_);

	InitializeCriticalSection(&this->format_critical_section_);
	InitializeConditionVariable(&this->format_cv_);

	InitializeCriticalSection(&this->delete_critical_section_);
	InitializeConditionVariable(&this->delete_cv_);

	InitializeCriticalSection(&this->file_critical_section_);
	InitializeConditionVariable(&this->file_cv_);
}

char KernelFS::mount(Partition* partition)
{
	if (partition == nullptr) return 0;

	this->is_format = true;
	EnterCriticalSection(&this->mount_critical_section_);
	
	while (this->partition_ != nullptr)
		SleepConditionVariableCS(&this->mount_cv_, &this->mount_critical_section_, INFINITE);
	
	this->partition_ = partition;
	
	this->bit_vector_clusters_cnt_ = 1 + this->partition_->getNumOfClusters() / (ClusterSize * CHAR_BIT);

	this->free_clusters_record_ = new FreeClustersRecord(this->bit_vector_clusters_cnt_);
	this->free_clusters_record_->read_from_partition(this->partition_);

	this->root_dir_index_ = new IndexCluster(this->bit_vector_clusters_cnt_);
	this->root_dir_index_->read_from_partition(this->partition_);

	this->cluster_allocator_ = new ClusterAllocator{ this->partition_ };
	
	this->cache_files_to_container();

	this->is_format = false;
	
	LeaveCriticalSection(&this->mount_critical_section_);
	
	return 1;
}

char KernelFS::unmount()
{
	if (this->partition_ == nullptr) return 0;

	EnterCriticalSection(&this->unmount_critical_section_);
	
	while (!this->opened_files_to_modes_map_.empty())
		SleepConditionVariableCS(&this->unmount_cv_, &this->unmount_critical_section_, INFINITE);
	
	delete this->cluster_allocator_;
	this->cluster_allocator_ = nullptr;
	
	this->root_dir_index_->write_to_partition(this->partition_);
	delete this->root_dir_index_;
	this->root_dir_index_ = nullptr;

	this->free_clusters_record_->write_to_partition(this->partition_);
	delete this->free_clusters_record_;
	this->free_clusters_record_ = nullptr;

	this->partition_ = nullptr;

	this->clear_cache();

	LeaveCriticalSection(&this->unmount_critical_section_);
	
	WakeConditionVariable(&this->mount_cv_);
	
	return 1;
}

char KernelFS::format()
{
	if (this->partition_ == nullptr) return 0;

	EnterCriticalSection(&this->format_critical_section_);

	while (!this->opened_files_to_modes_map_.empty())
		SleepConditionVariableCS(&this->format_cv_, &this->format_critical_section_, INFINITE);
	
	this->free_clusters_record_->format();
	this->root_dir_index_->format();
	this->clear_cache();

	LeaveCriticalSection(&this->format_critical_section_);
	
	return 1;
}

file_cnt_t KernelFS::get_number_of_files() const
{
	if (this->partition_ == nullptr) return -1;
	
	return this->files_.size();
}

char KernelFS::exists(char* filename)
{
	if (this->partition_ == nullptr) return 0;

	return this->files_.find(std::string{ KernelFS::to_dir_entry(filename).name }) != this->files_.end() ? 1 : 0;
}

dir_entry_t KernelFS::get_dir_entry(std::string filename)
{
	for (size_t i = 0; i < IndexCluster::clusters_count; i++)
	{
		if (this->root_dir_index_->get_cluster(i) != 0)
		{
			IndexCluster index2{ this->root_dir_index_->get_cluster(i) };
			index2.read_from_partition(this->partition_);
			for (size_t j = 0; j < IndexCluster::clusters_count; j++)
			{
				if (index2.get_cluster(j) != 0)
				{
					DirDataCluster* dir_data_cluster = this->cluster_allocator_->get_dir_data_cluster(index2.get_cluster(j));
					for (size_t k = 0; k < DirDataCluster::dir_entries_count; k++)
					{
						if (std::string{ dir_data_cluster->get_dir_entry(k).name } == filename)
						{
							return dir_data_cluster->get_dir_entry(k);
						}
					}
				}
			}
		}
	}

	// should never happen
	return {};
}

File* KernelFS::open(char* filename, char mode)
{
	if (this->is_format)
		return nullptr;
	// this->wait(std::string{ KernelFS::to_dir_entry(filename).name }, mode);

	this->readers_writers_.acquire(std::string{ KernelFS::to_dir_entry(filename).name }, mode == 'r');

	EnterCriticalSection(&this->file_critical_section_);
	
	while (this->opened_files_to_modes_map_.find(std::string{ KernelFS::to_dir_entry(filename).name })
		!= this->opened_files_to_modes_map_.end())
		SleepConditionVariableCS(&this->file_cv_, &this->file_critical_section_, INFINITE);
	
	if (mode == FileOperations::WRITE)
	{
		if (this->exists(filename))
			this->delete_file(filename);
		
		this->files_.emplace(KernelFS::to_dir_entry(filename).name);
		this->create_file_on_partition(KernelFS::to_dir_entry(filename));
	}

	if (!this->exists(filename))
	{
		this->readers_writers_.release(std::string{ KernelFS::to_dir_entry(filename).name }, mode == 'r');
		LeaveCriticalSection(&this->file_critical_section_);
		return nullptr;
	}

	this->opened_files_to_modes_map_[std::string{ KernelFS::to_dir_entry(filename).name }] = mode;

	dir_entry_t dir_entry = this->get_dir_entry(std::string{ KernelFS::to_dir_entry(filename).name });
	
	File* file = new File{};
	file->myImpl->set_partition(this->partition_);
	file->myImpl->set_dir_entry(dir_entry);
	file->myImpl->set_mode(mode);
	file->myImpl->cache_index_clusters();

	LeaveCriticalSection(&this->file_critical_section_);
	
	return file;
}

char KernelFS::delete_file(char* filename)
{
	if (this->exists(filename))
	{
		EnterCriticalSection(&this->delete_critical_section_);
		while (this->opened_files_to_modes_map_.find(std::string{ KernelFS::to_dir_entry(filename).name })
			!= this->opened_files_to_modes_map_.end())
			SleepConditionVariableCS(&this->delete_cv_, &this->delete_critical_section_, INFINITE);
		
		this->files_.erase(std::string{ KernelFS::to_dir_entry(filename).name });
		dir_entry_t dir_entry = KernelFS::to_dir_entry(filename);
		for (size_t i = 0; i < IndexCluster::clusters_count; i++)
		{
			if  (this->root_dir_index_->get_cluster(i) != 0)
			{
				IndexCluster index2{ this->root_dir_index_->get_cluster(i) };
				index2.read_from_partition(this->partition_);
				for (size_t j = 0; j < IndexCluster::clusters_count; j++)
				{
					if (index2.get_cluster(j) != 0)
					{
						DirDataCluster* dir_data_cluster = this->cluster_allocator_->get_dir_data_cluster(index2.get_cluster(j));
						for (size_t k = 0; k < DirDataCluster::dir_entries_count; k++)
						{
							if (KernelFS::is_same_descriptor(dir_entry, dir_data_cluster->get_dir_entry(k)))
							{
								dir_data_cluster->set_dir_entry(k, dir_entry_t{});
								LeaveCriticalSection(&this->delete_critical_section_);
								return 1;
							}
						}
					}
				}
			}
		}

		LeaveCriticalSection(&this->delete_critical_section_);
	}

	return 0;
}

ClusterAllocator* KernelFS::get_cluster_allocator() const
{
	return this->cluster_allocator_;
}

KernelFS* KernelFS::get_instance()
{
	return &kernel_fs_instance_;
}

void KernelFS::cache_files_to_container()
{
	for (size_t i = 0; i < IndexCluster::clusters_count; i++)
	{
		if (this->root_dir_index_->get_cluster(i) != 0)
		{
			IndexCluster index2_cluster{ this->root_dir_index_->get_cluster(i) };
			index2_cluster.read_from_partition(this->partition_);
			for (size_t j = 0; j < IndexCluster::clusters_count; j++)
			{
				if (index2_cluster.get_cluster(j) != 0)
				{
					DirDataCluster* dir_data_cluster = this->cluster_allocator_->get_dir_data_cluster(index2_cluster.get_cluster(j));
					for (size_t k = 0; k < DirDataCluster::dir_entries_count; k++)
					{
						dir_entry_t dir_entry = dir_data_cluster->get_dir_entry(k);
						if (dir_entry.name[0] != 0)
						{
							this->files_.emplace(dir_entry.name);
						}
					}
				}
			}
		}
	}
}

void KernelFS::clear_cache()
{
	this->files_.clear();
}

void KernelFS::create_file_on_partition(dir_entry_t dir_entry) const
{
	for (size_t i = 0; i < IndexCluster::clusters_count; i++)
	{
		bool is_allocated_index2 = false;
		if (this->root_dir_index_->get_cluster(i) == 0)
		{
			cluster_number_t next_free_cluster = this->free_clusters_record_->get_next_free_cluster_number();
			this->free_clusters_record_->allocate_cluster(next_free_cluster);
			this->free_clusters_record_->write_to_partition(this->partition_);
			this->root_dir_index_->set_cluster(i, next_free_cluster);
			this->root_dir_index_->write_to_partition(this->partition_);
			is_allocated_index2 = true;
		}

		IndexCluster index2{ this->root_dir_index_->get_cluster(i) };
		if (is_allocated_index2)
			index2.format();
		else
			index2.read_from_partition(this->partition_);
		
		for (size_t j = 0; j < IndexCluster::clusters_count; j++)
		{
			bool dir_data_cluster_allocated = false;
			if (index2.get_cluster(j) == 0)
			{
				cluster_number_t next_free_cluster = this->free_clusters_record_->get_next_free_cluster_number();
				this->free_clusters_record_->allocate_cluster(next_free_cluster);
				this->free_clusters_record_->write_to_partition(this->partition_);
				index2.set_cluster(j, next_free_cluster);
				index2.write_to_partition(this->partition_);
				dir_data_cluster_allocated = true;
			}

			DirDataCluster* dir_data_cluster = this->cluster_allocator_->get_dir_data_cluster(index2.get_cluster(j));
			if (dir_data_cluster_allocated)
				dir_data_cluster->format();
			
			size_t free_entry = dir_data_cluster->get_free_entry();
			if (free_entry < DirDataCluster::dir_entries_count)
			{
				dir_entry.cluster_number = index2.get_cluster(j);
				dir_entry.offset_in_cluster = free_entry;
				dir_data_cluster->set_dir_entry(free_entry, dir_entry);
				return;
			}
		}
	}
}

bool KernelFS::is_same_descriptor(dir_entry_t dir_entry1, dir_entry_t dir_entry2)
{
	return std::string{ dir_entry1.name } == std::string{ dir_entry2.name };
}

void KernelFS::wait(std::string filename, char mode)
{
	if (this->open_files_readers_writers_.find(filename) != this->open_files_readers_writers_.end())
	{
		/*
		if (this->opened_files_to_modes_map_[filename] == 'r' && mode == 'r')
		{
			this->opened_files_to_cnt_map_[filename] += 1;
			return;
		}
		*/
		HANDLE sem = this->open_files_readers_writers_[filename];
		WaitForSingleObject(sem, INFINITE);
	}
	else
	{
		HANDLE sem = CreateSemaphore(NULL, 0, 32, NULL);
		this->open_files_readers_writers_[filename] = sem;
		//this->opened_files_to_cnt_map_[filename] = 1;
	}
}

void KernelFS::signal(std::string filename)
{
	if (this->open_files_readers_writers_.find(filename) != this->open_files_readers_writers_.end())
	{
		HANDLE sem = this->open_files_readers_writers_[filename];
		//this->opened_files_to_cnt_map_[filename]--;
		//if (this->opened_files_to_cnt_map_[filename] == 0)
		//{
			this->open_files_readers_writers_.erase(filename);
			//this->opened_files_to_cnt_map_.erase(filename);
			ReleaseSemaphore(sem, 1, NULL);
		//}
	}
}

FreeClustersRecord* KernelFS::get_free_clusters_record() const
{
	return this->free_clusters_record_;
}

/**
 * \brief Creates an empty dir_entry_t based on the filepath
 * \param filepath starts with /, has max 8 chars for name followed by . and max 3 chars for ext ("/example.ext")
 * \return the dir_entry_t corresponding to the empty file with the given name
 */
dir_entry_t KernelFS::to_dir_entry(char* filepath)
{
	dir_entry_t dir_entry;

	for (char& i : dir_entry.name)
		i = ' ';

	for (char& i : dir_entry.extension)
		i = ' ';
		
	size_t i = 0;
	for (; i < FNAMELEN && filepath[i + 1] != '.'; i++)
		dir_entry.name[i] = filepath[i + 1];

	for (size_t j = i; j < i + FEXTLEN && filepath[j + 2] != '\0'; j++)
		dir_entry.extension[j - i] = filepath[j + 2];
	
	return dir_entry;
}

void KernelFS::close_file(std::string filename, char mode)
{
	this->opened_files_to_modes_map_.erase(filename);
	// this->signal(filename);
	this->readers_writers_.release(filename, mode == 'r');

	WakeConditionVariable(&this->unmount_cv_);
	WakeConditionVariable(&this->format_cv_);
	WakeConditionVariable(&this->delete_cv_);
	WakeAllConditionVariable(&this->file_cv_);
}

/**
 *	Only instance which is required by the FS interface
 *	(the Singleton design pattern was not implemented)
 */
KernelFS KernelFS::kernel_fs_instance_;
