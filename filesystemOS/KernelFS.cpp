#include "KernelFS.h"
#include "DirDataCluster.h"
#include "File.h"

char KernelFS::mount(Partition* partition)
{
	if (partition == nullptr) return 0;
	
	this->partition_ = partition;
	
	this->bit_vector_clusters_cnt_ = 1 + this->partition_->getNumOfClusters() / (ClusterSize * CHAR_BIT);

	this->free_clusters_record_ = new FreeClustersRecord(this->bit_vector_clusters_cnt_);
	this->free_clusters_record_->read_from_partition(this->partition_);

	this->root_dir_index_ = new IndexCluster(this->bit_vector_clusters_cnt_);
	this->root_dir_index_->read_from_partition(this->partition_);

	this->cluster_allocator_ = new ClusterAllocator{ this->partition_ };
	
	this->cache_files_to_container();

	return 1;
}

char KernelFS::unmount()
{
	if (this->partition_ == nullptr) return 0;

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
	
	return 1;
}

char KernelFS::format()
{
	if (this->partition_ == nullptr) return 0;
	
	this->free_clusters_record_->format();
	this->root_dir_index_->format();
	this->clear_cache();
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

File* KernelFS::open(char* filename, char mode)
{
	// TODO: synchronization
	if (mode == FileOperations::WRITE)
	{
		if (this->exists(filename))
			this->delete_file(filename);
		
		this->files_.emplace(KernelFS::to_dir_entry(filename).name);
		this->create_file_on_partition(KernelFS::to_dir_entry(filename));
	}

	if (!this->exists(filename))
		return nullptr;

	this->opened_files_to_modes_map_[std::string{ KernelFS::to_dir_entry(filename).name }] = mode;

	File* file = new File{};
	// TODO: file opening
	return file;
}

char KernelFS::delete_file(char* filename)
{
	// TODO: check open files
	if (this->exists(filename))
	{
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
								return 1;
							}
						}
					}
				}
			}
		}
	}

	return 0;
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

/**
 *	Only instance which is required by the FS interface
 *	(the Singleton design pattern was not implemented)
 */
KernelFS KernelFS::kernel_fs_instance_;
