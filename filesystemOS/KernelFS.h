#pragma once

#include "part.h"
#include "FS.h"

#include <unordered_set>
#include <map>
#include <iostream>
#include "IndexCluster.h"
#include "FreeClustersRecord.h"

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

	file_cnt_t number_of_files();

	char exists(char* filename);

	File* open(char* filename, char mode);

	char delete_file(char* filename);

	static KernelFS* get_instance();
protected:
	static KernelFS kernel_fs_instance_;
private:
	Partition* partition_ = nullptr;

	cluster_cnt_t bit_vector_clusters_cnt_ = 0;
	FreeClustersRecord* free_clusters_record_ = nullptr;

	IndexCluster* root_dir_index_ = nullptr;

	std::unordered_set<char*> files_;
	std::map<char*, char> opened_files_to_modes_map_;
};

// extern KernelFS kernelFS_instance;
