#include "FreeClustersRecord.h"

FreeClustersRecord::FreeClustersRecord(size_t number_of_clusters)
	: number_of_clusters_(number_of_clusters)
{
	this->bit_vector_clusters_ = new BitVectorCluster[number_of_clusters];
	for (size_t i = 0; i < number_of_clusters; i++)
		this->bit_vector_clusters_[i].set_cluster_number(i);
}

void FreeClustersRecord::read_from_partition(Partition* partition)
{
	for (size_t i = 0; i < this->number_of_clusters_; i++)
		this->bit_vector_clusters_[i].read_from_partition(partition);
}

FreeClustersRecord::~FreeClustersRecord()
{
	delete [] this->bit_vector_clusters_;
	this->bit_vector_clusters_ = nullptr;
}
