#include "IndexCluster.h"

IndexCluster::IndexCluster(cluster_number_t cluster_number)
	: Cluster(cluster_number)
{
	// empty body
}

void IndexCluster::set_cluster(size_t index, uint32_t cluster)
{
	this->dirty_ = true;
	this->clusters_[index] = cluster;
}

uint32_t IndexCluster::get_cluster(size_t index) const
{
	return this->clusters_[index];
}

void IndexCluster::format()
{
	this->dirty_ = true;
	for (size_t i = 0; i < ClusterSize; i++)
		this->buffer_[i] = 0;
}
