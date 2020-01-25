#include "ClusterAllocator.h"

ClusterAllocator::ClusterAllocator(Partition* partition)
	: partition_(partition)
{
	// empty body
}

ClusterAllocator::~ClusterAllocator()
{
	this->clear();
}

ClusterAllocator::DataCluster* ClusterAllocator::get_data_cluster(cluster_number_t cluster_number)
{
	const auto it = this->data_clusters_.find(cluster_number);
	if (it != this->data_clusters_.end())
		return it->second;

	this->data_clusters_[cluster_number] = new DataCluster{ cluster_number };
	this->data_clusters_[cluster_number]->read_from_partition(this->partition_);
	return this->data_clusters_[cluster_number];
}

DirDataCluster* ClusterAllocator::get_dir_data_cluster(cluster_number_t cluster_number)
{
	const auto it = this->dir_data_clusters_.find(cluster_number);
	if (it != this->dir_data_clusters_.end())
		return  it->second;

	this->dir_data_clusters_[cluster_number] = new DirDataCluster{ cluster_number };
	this->dir_data_clusters_[cluster_number]->read_from_partition(this->partition_);
	return this->dir_data_clusters_[cluster_number];
}

void ClusterAllocator::clear()
{
	for (auto elem : data_clusters_)
	{
		elem.second->write_to_partition(this->partition_);
		delete elem.second;
	}
	
	data_clusters_.clear();
	
	for (auto elem : dir_data_clusters_)
	{
		elem.second->write_to_partition(this->partition_);
		delete elem.second;
	}
	
	dir_data_clusters_.clear();
}
