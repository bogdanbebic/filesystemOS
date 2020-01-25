#pragma once

#include "Cluster.h"
#include "DirDataCluster.h"
#include <map>

class ClusterAllocator
{
public:
	explicit ClusterAllocator(Partition* partition);
	~ClusterAllocator();
	
	// Will maybe changed later
	using DataCluster = Cluster;
	
	DataCluster* get_data_cluster(cluster_number_t cluster_number);
	DirDataCluster* get_dir_data_cluster(cluster_number_t cluster_number);

	void clear();
private:
	Partition* partition_ = nullptr;
	
	std::map<cluster_number_t, DataCluster*> data_clusters_;
	std::map<cluster_number_t, DirDataCluster*> dir_data_clusters_;
};

