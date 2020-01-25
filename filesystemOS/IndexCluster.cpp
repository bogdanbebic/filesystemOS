#include "IndexCluster.h"

#include <cstring>

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
	std::memset(this->buffer_, 0, sizeof this->buffer_);
}

size_t IndexCluster::get_free_entry() const
{
	for (size_t i = 0; i < IndexCluster::clusters_count; i++)
		if (this->get_cluster(i) == 0)
			return i;

	return IndexCluster::clusters_count;
}
