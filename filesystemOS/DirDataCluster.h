#pragma once
#include "Cluster.h"

#include "DirEntry.h"

class DirDataCluster : public Cluster
{
public:
	explicit DirDataCluster(cluster_number_t cluster_number);

	void set_dir_entry(size_t index, dir_entry_t dir_entry);
	dir_entry_t get_dir_entry(size_t index) const;

	static const size_t dir_entries_count = ClusterSize / sizeof(dir_entry_t);
private:
	dir_entry_t* dir_entries_ = (dir_entry_t*)this->buffer_;
};

