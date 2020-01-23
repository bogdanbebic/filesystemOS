#pragma once
#include "Cluster.h"

#include <stdint.h>


class IndexCluster : public Cluster
{
public:
	explicit IndexCluster(cluster_number_t cluster_number);

	void set_cluster(size_t index, uint32_t cluster);
	uint32_t get_cluster(size_t index) const;

	void format();

	static const size_t clusters_count = ClusterSize / sizeof(uint32_t);
private:
	uint32_t* clusters_ = (uint32_t*)this->buffer_;
};

