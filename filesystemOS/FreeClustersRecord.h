#pragma once

#include "BitVectorCluster.h"

class FreeClustersRecord
{
public:
	explicit FreeClustersRecord(size_t number_of_clusters);

	~FreeClustersRecord();

	void read_from_partition(Partition* partition) const;
	void write_to_partition(Partition* partition) const;

	cluster_number_t get_next_free_cluster_number() const;

	void allocate_cluster(cluster_number_t cluster_number) const;
	void deallocate_cluster(cluster_number_t cluster_number) const;

	void format() const;
private:
	size_t number_of_clusters_ = 0;
	BitVectorCluster* bit_vector_clusters_ = nullptr;
};
