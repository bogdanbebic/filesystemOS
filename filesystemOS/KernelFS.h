#pragma once

#include "part.h"
#include "FS.h"

#include <unordered_set>
#include <map>
#include <iostream>
#include "IndexCluster.h"
#include "FreeClustersRecord.h"
#include "DirEntry.h"
#include "ClusterAllocator.h"

using file_cnt_t = FileCnt;
using cluster_cnt_t = ClusterNo;

class KernelFS
{
public:
	enum FileOperations { WRITE = 'w', READ = 'r', APPEND = 'a' };
	// TODO: delete ctor and dtor
	KernelFS() { std::cout << "KernelFS ctor\n"; }
	~KernelFS() { std::cout << "KernelFS dtor\n"; }

	char mount(Partition* partition);
	
	char unmount();

	char format();

	file_cnt_t get_number_of_files() const;

	char exists(char* filename);

	File* open(char* filename, char mode);

	char delete_file(char* filename);

	ClusterAllocator* get_cluster_allocator() const;
	FreeClustersRecord* get_free_clusters_record() const;

	static KernelFS* get_instance();
	static dir_entry_t to_dir_entry(char* filepath);
protected:
	static KernelFS kernel_fs_instance_;
private:
	void cache_files_to_container();
	void clear_cache();

	dir_entry_t get_dir_entry(std::string filename);
	
	void create_file_on_partition(dir_entry_t dir_entry) const;

	static bool is_same_descriptor(dir_entry_t dir_entry1, dir_entry_t dir_entry2);
	
	Partition* partition_ = nullptr;

	cluster_cnt_t bit_vector_clusters_cnt_ = 0;
	FreeClustersRecord* free_clusters_record_ = nullptr;

	IndexCluster* root_dir_index_ = nullptr;


	ClusterAllocator* cluster_allocator_ = nullptr;

	std::unordered_set<std::string> files_;
	std::map<std::string, char> opened_files_to_modes_map_;
};

// extern KernelFS kernelFS_instance;
