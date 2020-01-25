#include "Cluster.h"

#include <cstring>

Cluster::Cluster(cluster_number_t cluster_number)
	: cluster_number_{cluster_number}
{
	// empty body
}

void Cluster::set_cluster_number(cluster_number_t cluster_number)
{
	this->cluster_number_ = cluster_number;
}

cluster_number_t Cluster::get_cluster_number() const
{
	return this->cluster_number_;
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

void Cluster::format()
{
	this->dirty_ = true;
	std::memset(this->buffer_, 0, sizeof this->buffer_);
}
