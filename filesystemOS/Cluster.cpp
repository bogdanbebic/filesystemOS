#include "Cluster.h"

Cluster::Cluster(cluster_number_t cluster_number)
	: cluster_number_{cluster_number}
{
	// empty body
}

void Cluster::set_cluster_number(cluster_number_t cluster_number)
{
	this->cluster_number_ = cluster_number;
}

void Cluster::read_from_partition(Partition* partition)
{
	partition->readCluster(this->cluster_number_, this->buffer_);
	this->is_dirty_ = false;
}

void Cluster::write_to_partition(Partition* partition)
{
	if (this->is_dirty_)
		partition->writeCluster(this->cluster_number_, this->buffer_);
	this->is_dirty_ = false;
}
