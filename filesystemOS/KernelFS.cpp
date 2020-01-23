#include "KernelFS.h"
#include "DirDataCluster.h"

char KernelFS::mount(Partition* partition)
{
	if (partition == nullptr) return 0;
	
	this->partition_ = partition;
	
	this->bit_vector_clusters_cnt_ = 1 + this->partition_->getNumOfClusters() / (ClusterSize * CHAR_BIT);

	this->free_clusters_record_ = new FreeClustersRecord(this->bit_vector_clusters_cnt_);
	this->free_clusters_record_->read_from_partition(this->partition_);

	this->root_dir_index_ = new IndexCluster(this->bit_vector_clusters_cnt_);
	this->root_dir_index_->read_from_partition(this->partition_);

	this->cache_files_to_container();

	return 1;
}

char KernelFS::unmount()
{
	if (this->partition_ == nullptr) return 0;
	
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

file_cnt_t KernelFS::number_of_files() const
{
	// TODO: check failure
	return this->files_.size();
}

char KernelFS::exists(char* filename)
{
	return this->files_.find(filename) != this->files_.end() ? 1 : 0;
}

File* KernelFS::open(char* filename, char mode)
{
	if (mode == FileOperations::WRITE)
	{
		if (!this->exists(filename))
		{
			this->files_.insert(filename);
			// TODO: disk operation
		}
		else
		{
			// TODO: delete contents of existing file from disk
		}
	}

	if (!this->exists(filename))
		return nullptr;

	this->opened_files_to_modes_map_[filename] = mode;

	// TODO: file opening
	return nullptr;
}

char KernelFS::delete_file(char* filename)
{
	// TODO: check open files
	if (this->exists(filename))
	{
		this->files_.erase(filename);
		// TODO: disk operation
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
					DirDataCluster dir_data_cluster{ index2_cluster.get_cluster(j) };
					dir_data_cluster.read_from_partition(partition_);
					for (size_t k = 0; k < DirDataCluster::dir_entries_count; k++)
					{
						dir_entry_t dir_entry = dir_data_cluster.get_dir_entry(k);
						if (dir_entry.name[0] != 0)
						{
							std::string name{ dir_entry.name };
							std::string ext{ dir_entry.extension };
							std::string filepath = name + "." + ext;
							this->files_.insert(filepath.c_str());
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

/**
 *	Only instance which is required by the FS interface
 *	(the Singleton design pattern was not implemented)
 */
KernelFS KernelFS::kernel_fs_instance_;
