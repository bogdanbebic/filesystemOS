#include "Cluster.h"

Cluster::Cluster(cluster_number_t cluster_number)
	: cluster_number_{cluster_number}
{
	add_to_cache(this);
}

Cluster::~Cluster()
{
	remove_from_cache(this);
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

void Cluster::add_to_cache(Cluster* cluster)
{
	if (cached_clusters_.size() > cache_size)
		remove_from_cache(get_victim_cluster());
	
	cached_clusters_.push_back(cluster);
}

void Cluster::remove_from_cache(Cluster* cluster)
{
	// TODO: fix memory leak
	cached_clusters_.remove(cluster);
}

Cluster* Cluster::get_victim_cluster()
{
	return *cached_clusters_.begin();
}
