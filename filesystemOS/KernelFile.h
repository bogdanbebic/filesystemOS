#pragma once

#include "FS.h"
#include "DirEntry.h"
#include "IndexCluster.h"
#include <map>

using bytes_cnt_t = BytesCnt;

class KernelFile
{
public:
	~KernelFile();
	
	char write(bytes_cnt_t bytes_cnt, char* buffer);
	bytes_cnt_t read(bytes_cnt_t bytes_cnt, char* buffer);
	
	char seek(bytes_cnt_t new_position);

	bytes_cnt_t get_current_position() const;

	char eof() const;

	bytes_cnt_t get_file_size() const;

	char truncate();

	void set_dir_entry(dir_entry_t dir_entry);
	void set_mode(char mode);
	void set_partition(Partition* partition);
	void cache_index_clusters();
private:
	size_t get_offset_in_data_cluster() const;
	cluster_number_t get_current_data_cluster_number();
	Cluster* get_current_data_cluster();

	void extend(size_t size_increment);

	bytes_cnt_t current_position_ = 0;
	bytes_cnt_t size_ = 0;

	bytes_cnt_t max_size_ = 0;
	
	IndexCluster* file_index_ = nullptr;
	std::map<cluster_number_t, IndexCluster*> file_index2_;

	char mode_ = 'r';
	
	dir_entry_t dir_entry_{};

	Partition* partition_ = nullptr;
};
