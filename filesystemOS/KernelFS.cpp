#include "KernelFS.h"

char KernelFS::mount(Partition* partition)
{
	if (partition == nullptr) return -1;
	
	this->partition_ = partition;
	
	this->bit_vector_clusters_cnt_ = 1 + this->partition_->getNumOfClusters() / (ClusterSize * CHAR_BIT);

	this->free_clusters_record_ = new FreeClustersRecord(this->bit_vector_clusters_cnt_);
	this->free_clusters_record_->read_from_partition(this->partition_);

	this->root_dir_index_ = new IndexCluster(this->bit_vector_clusters_cnt_);
	this->root_dir_index_->read_from_partition(this->partition_);
	return 0;
}

char KernelFS::unmount()
{
	this->root_dir_index_->write_to_partition(this->partition_);
	delete this->root_dir_index_;
	this->root_dir_index_ = nullptr;

	this->free_clusters_record_->write_to_partition(this->partition_);
	delete this->free_clusters_record_;
	this->free_clusters_record_ = nullptr;

	this->partition_ = nullptr;
	return 0;
}

char KernelFS::format()
{
	this->free_clusters_record_->format();
	this->root_dir_index_->format();
	return 0;
}

file_cnt_t KernelFS::number_of_files()
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

/**
 *	Only instance which is required by the FS interface
 *	(the Singleton design pattern was not implemented)
 */
KernelFS KernelFS::kernel_fs_instance_;
