#pragma once
#include "Cluster.h"

class BitVectorCluster : public Cluster
{
public:
	explicit BitVectorCluster(cluster_number_t cluster_number = 0);

	void set_bit(size_t index, bool value);
	bool get_bit(size_t index) const;

	void format();
private:
	void set_bit(size_t index);
	void clear_bit(size_t index);
};

