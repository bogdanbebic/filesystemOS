#pragma once

#include "part.h"

using cluster_number_t = ClusterNo;

class Cluster
{
public:
	explicit Cluster(cluster_number_t cluster_number);

	void set_cluster_number(cluster_number_t cluster_number);
	void read_from_partition(Partition* partition);
	void write_to_partition(Partition* partition);

	void set_char_in_buffer(size_t index, char ch);
	char get_char_from_buffer(size_t index) const;
protected:
	// left for use by subclasses, serves only as a structure

	bool dirty_ = false;
	char buffer_[ClusterSize] = {};
private:
	cluster_number_t cluster_number_;
};
