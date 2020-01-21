#include "DirDataCluster.h"

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
