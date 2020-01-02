#include "KernelFS.h"

char KernelFS::mount(Partition* partition)
{
	return 0;
}

char KernelFS::unmount()
{
	return 0;
}

char KernelFS::format()
{
	return 0;
}

file_cnt_t KernelFS::number_of_files()
{
	// TODO: check failiure
	return this->files_.size();
}

char KernelFS::exists(char* filename)
{
	return this->files_.find(filename) != this->files_.end() ? 1 : 0;
}

File* KernelFS::open(char* filename, char mode)
{
	return nullptr;
}

char KernelFS::delete_file(char* fname)
{
	return 0;
}

KernelFS* KernelFS::get_instance()
{
	return &kernelFS_instance;
}

/**
 *	Only instance which is required by the FS interface
 *	(the Singleton design pattern was not implemented)
 */
KernelFS KernelFS::kernelFS_instance;
