#include "KernelFile.h"

#include "KernelFS.h"

KernelFile::~KernelFile()
{
	this->dir_entry_.file_size = this->size_;
	if (this->file_index_ != nullptr)
		this->dir_entry_.first_level_index_of_file = this->file_index_->get_cluster_number();

	DirDataCluster* dir_data_cluster = KernelFS::get_instance()->get_cluster_allocator()->get_dir_data_cluster(this->dir_entry_.cluster_number);
	dir_data_cluster->set_dir_entry(this->dir_entry_.offset_in_cluster, this->dir_entry_);
	
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
	
	if (this->size_ <= this->current_position_ + bytes_cnt)
		this->extend(this->current_position_ + bytes_cnt - this->size_);
	
	bytes_cnt_t i = 0;
	while (i < bytes_cnt && !this->eof())
	{
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
		Cluster* current_data_cluster = this->get_current_data_cluster();
		buffer[i] = current_data_cluster->get_char_from_buffer(this->get_offset_in_data_cluster());
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
	this->size_ = dir_entry.file_size;
}

void KernelFile::set_mode(char mode)
{
	this->mode_ = mode;
	if (mode == 'a')
		this->current_position_ = this->size_;
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

void KernelFile::extend(size_t size_increment)
{
	size_t new_size = this->size_ + size_increment;

	while (new_size > this->max_size_)
	{
		if (this->file_index_ == nullptr)
		{
			cluster_number_t next_free_cluster = KernelFS::get_instance()->get_free_clusters_record()->get_next_free_cluster_number();
			KernelFS::get_instance()->get_free_clusters_record()->allocate_cluster(next_free_cluster);
			this->file_index_ = new IndexCluster{ next_free_cluster };
			this->file_index_->format();
		}

		cluster_number_t i = 0;
		bool is_cluster_allocated = false;
		while (this->file_index_->get_cluster(i) != 0)
		{
			IndexCluster index2{ this->file_index_->get_cluster(i) };
			index2.read_from_partition(this->partition_);
			size_t free_entry = index2.get_free_entry();
			if (free_entry != IndexCluster::clusters_count)
			{
				cluster_number_t next_free_cluster = KernelFS::get_instance()->get_free_clusters_record()->get_next_free_cluster_number();
				KernelFS::get_instance()->get_free_clusters_record()->allocate_cluster(next_free_cluster);

				Cluster* data_cluster = KernelFS::get_instance()->get_cluster_allocator()->get_data_cluster(next_free_cluster);
				data_cluster->format();
				index2.set_cluster(free_entry, next_free_cluster);
				index2.write_to_partition(this->partition_);
				is_cluster_allocated = true;
				break;
			}
			
			i++;
		}

		if (!is_cluster_allocated)
		{
			cluster_number_t next_free_cluster = KernelFS::get_instance()->get_free_clusters_record()->get_next_free_cluster_number();
			KernelFS::get_instance()->get_free_clusters_record()->allocate_cluster(next_free_cluster);
			this->file_index_->set_cluster(i, next_free_cluster);
			this->file_index_->write_to_partition(this->partition_);
			IndexCluster* index2 = new IndexCluster{ next_free_cluster };
			this->file_index2_[next_free_cluster] = index2;
			index2->format();
			size_t free_entry = index2->get_free_entry();
			if (free_entry != IndexCluster::clusters_count)
			{
				cluster_number_t next_free_cluster = KernelFS::get_instance()->get_free_clusters_record()->get_next_free_cluster_number();
				KernelFS::get_instance()->get_free_clusters_record()->allocate_cluster(next_free_cluster);

				Cluster* data_cluster = KernelFS::get_instance()->get_cluster_allocator()->get_data_cluster(next_free_cluster);
				data_cluster->format();
				index2->set_cluster(free_entry, next_free_cluster);
				index2->write_to_partition(this->partition_);
			}
		}
		
		this->max_size_ += ClusterSize;
	}
	
	this->size_ = new_size;
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
				if (this->file_index2_.find(cluster_number_index2) == this->file_index2_.end())
				{
					IndexCluster* index2 = new IndexCluster{ cluster_number_index2 };
					index2->read_from_partition(this->partition_);
					this->file_index2_[cluster_number_index2] = index2;
				}
			}
		}
	}
}
