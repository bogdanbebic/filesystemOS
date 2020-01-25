#include "Cluster.h"

Cluster::Cluster(cluster_number_t cluster_number)
	: cluster_number_{cluster_number}
{
	// add_to_cache(this);
}

Cluster::~Cluster()
{
	// remove_from_cache(this);
}

void Cluster::set_cluster_number(cluster_number_t cluster_number)
{
	this->cluster_number_ = cluster_number;
}

void Cluster::read_from_partition(Partition* partition)
{
	partition->readCluster(this->cluster_number_, this->buffer_);
	this->dirty_ = false;
}

void Cluster::write_to_partition(Partition* partition)
{
	if (this->dirty_)
		partition->writeCluster(this->cluster_number_, this->buffer_);
	this->dirty_ = false;
}

void Cluster::set_char_in_buffer(size_t index, char ch)
{
	this->dirty_ = true;
	this->buffer_[index] = ch;
}

char Cluster::get_char_from_buffer(size_t index) const
{
	return this->buffer_[index];
}
