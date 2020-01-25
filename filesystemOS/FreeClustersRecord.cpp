#include "FreeClustersRecord.h"
#include <climits>

FreeClustersRecord::FreeClustersRecord(size_t number_of_clusters)
	: number_of_clusters_(number_of_clusters)
{
	this->bit_vector_clusters_ = new BitVectorCluster[number_of_clusters];
	for (size_t i = 0; i < number_of_clusters; i++)
		this->bit_vector_clusters_[i].set_cluster_number(i);
}

void FreeClustersRecord::read_from_partition(Partition* partition) const
{
	for (size_t i = 0; i < this->number_of_clusters_; i++)
		this->bit_vector_clusters_[i].read_from_partition(partition);
}

void FreeClustersRecord::write_to_partition(Partition* partition) const
{
	for (size_t i = 0; i < this->number_of_clusters_; i++)
		this->bit_vector_clusters_[i].write_to_partition(partition);
}

cluster_number_t FreeClustersRecord::get_next_free_cluster_number() const
{
	for (size_t i = 0; i < this->number_of_clusters_; i++)
		for (size_t j = 0; j < ClusterSize * CHAR_BIT; j++)
			if (this->bit_vector_clusters_[i].get_bit(j))
				return j + i * ClusterSize * CHAR_BIT;

	return 0;
}

void FreeClustersRecord::allocate_cluster(cluster_number_t cluster_number) const
{
	const size_t index_of_bit_clusters = cluster_number / (ClusterSize * CHAR_BIT);
	const size_t index_of_bit_in_cluster = cluster_number % (ClusterSize * CHAR_BIT);
	this->bit_vector_clusters_[index_of_bit_clusters].set_bit(index_of_bit_in_cluster, false);
}

void FreeClustersRecord::deallocate_cluster(cluster_number_t cluster_number) const
{
	const size_t index_of_bit_clusters = cluster_number / (ClusterSize * CHAR_BIT);
	const size_t index_of_bit_in_cluster = cluster_number % (ClusterSize * CHAR_BIT);
	this->bit_vector_clusters_[index_of_bit_clusters].set_bit(index_of_bit_in_cluster, true);
}

void FreeClustersRecord::format() const
{
	for (size_t i = 0; i < this->number_of_clusters_; i++)
		this->bit_vector_clusters_[i].format();

	for (size_t i = 0; i < this->number_of_clusters_; i++)
		this->allocate_cluster(i);

	this->allocate_cluster(this->number_of_clusters_);
}

FreeClustersRecord::~FreeClustersRecord()
{
	delete [] this->bit_vector_clusters_;
	this->bit_vector_clusters_ = nullptr;
}
