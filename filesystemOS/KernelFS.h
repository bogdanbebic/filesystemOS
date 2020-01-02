#pragma once

#include "part.h"
#include "FS.h"

#include <unordered_set>
#include <map>
#include <iostream>

using file_cnt_t = FileCnt;

class KernelFS
{
public:
	// TODO: delete ctor and dtor
	KernelFS() { std::cout << "KernelFS ctor\n"; }
	~KernelFS() { std::cout << "KernelFS dtor\n"; }

	char mount(Partition* partition);
	
	char unmount();

	char format();

	file_cnt_t number_of_files();

	char exists(char* filename);

	File* open(char* filename, char mode);

	char delete_file(char* fname);

	static KernelFS* get_instance();
protected:
	static KernelFS kernelFS_instance;
private:
	std::unordered_set<char*> files_;
	std::map<char*, char> opened_files_to_modes_map_;
};

// extern KernelFS kernelFS_instance;
