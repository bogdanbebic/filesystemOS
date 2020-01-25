#include "KernelFile.h"

#include "KernelFS.h"

KernelFile::~KernelFile()
{
	if (this->file_index_ != nullptr)
	{
		this->file_index_->write_to_partition(this->partition_);
		delete this->file_index_;
	}

	for (auto &index2_pair : this->file_index2_)
	{
		index2_pair.second->write_to_partition(this->partition_);
		delete index2_pair.second;
	}

	this->file_index2_.clear();
}

char KernelFile::write(bytes_cnt_t bytes_cnt, char* buffer)
{
	if (this->mode_ == 'r')
		return 0;
	
	bytes_cnt_t i = 0;
	while (i < bytes_cnt)
	{
		if (this->eof())
		{
			// TODO: allocate one more cluster
			return 1;
		}
		
		this->get_current_data_cluster()->set_char_in_buffer(this->get_offset_in_data_cluster(), buffer[i]);
		i++;
		this->current_position_++;
	}
		
	return 1;
}

bytes_cnt_t KernelFile::read(bytes_cnt_t bytes_cnt, char* buffer)
{
	if (this->mode_ != 'r')
		return 0;

	bytes_cnt_t i = 0;

	while (i < bytes_cnt && !this->eof())
	{
		buffer[i] = this->get_current_data_cluster()->get_char_from_buffer(this->get_offset_in_data_cluster());
		i++;
		this->current_position_++;
	}
		
	return i;
}

char KernelFile::seek(bytes_cnt_t new_position)
{
	if (new_position > this->size_)
		return 0;

	this->current_position_ = new_position;
	return 1;
}

bytes_cnt_t KernelFile::get_current_position() const
{
	return this->current_position_;
}

/**
 * \brief Checks if current position is at end of file
 * \return 0 - no, 1 - error, 2 - yes
 */
char KernelFile::eof() const
{
	return this->current_position_ == this->size_ ? 2 : 0;
}

bytes_cnt_t KernelFile::get_file_size() const
{
	return this->size_;
}

char KernelFile::truncate()
{
	// TODO: implement
	return 1;
}

void KernelFile::set_dir_entry(dir_entry_t dir_entry)
{
	this->dir_entry_ = dir_entry;
}

void KernelFile::set_mode(char mode)
{
	this->mode_ = mode;
}

void KernelFile::set_partition(Partition* partition)
{
	this->partition_ = partition;
}

size_t KernelFile::get_offset_in_data_cluster() const
{
	return this->current_position_ % ClusterSize;
}

cluster_number_t KernelFile::get_current_data_cluster_number()
{
	const cluster_number_t index2 = this->file_index_->get_cluster(this->current_position_ / ClusterSize / IndexCluster::clusters_count);
	const cluster_number_t data_cluster = this->file_index2_[index2]->get_cluster(this->current_position_ / ClusterSize);
	return data_cluster;
}

Cluster* KernelFile::get_current_data_cluster()
{
	cluster_number_t current = this->get_current_data_cluster_number();
	if (current == 0)
		return nullptr;

	return KernelFS::get_instance()->get_cluster_allocator()->get_data_cluster(current);
}

void KernelFile::cache_index_clusters()
{
	if (this->dir_entry_.first_level_index_of_file != 0)
	{
		if (this->file_index_ == nullptr)
		{
			this->file_index_ = new IndexCluster{ this->dir_entry_.first_level_index_of_file };
			this->file_index_->read_from_partition(this->partition_);
		}
		
		for (size_t i = 0; i < IndexCluster::clusters_count; i++)
		{
			cluster_number_t cluster_number_index2 = this->file_index_->get_cluster(i);
			if (cluster_number_index2 != 0)
			{
				if (this->file_index2_.find(cluster_number_index2) != this->file_index2_.end())
				{
					IndexCluster* index2 = new IndexCluster{ cluster_number_index2 };
					index2->read_from_partition(this->partition_);
					this->file_index2_[cluster_number_index2] = index2;					
				}
			}
		}
	}
}
