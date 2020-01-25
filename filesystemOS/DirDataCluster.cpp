#include "DirDataCluster.h"

#include <cstring>

DirDataCluster::DirDataCluster(cluster_number_t cluster_number)
	: Cluster(cluster_number)
{
	// empty body
}

void DirDataCluster::set_dir_entry(size_t index, dir_entry_t dir_entry)
{
	this->dirty_ = true;
	this->dir_entries_[index] = dir_entry;
}

dir_entry_t DirDataCluster::get_dir_entry(size_t index) const
{
	return this->dir_entries_[index];
}

size_t DirDataCluster::get_free_entry() const
{
	for (size_t i = 0; i < DirDataCluster::dir_entries_count; i++)
		if (this->get_dir_entry(i).name[0] == 0)
			return i;

	return DirDataCluster::dir_entries_count;
}

void DirDataCluster::format()
{
	this->dirty_ = true;
	std::memset(this->buffer_, 0, sizeof this->buffer_);
}
