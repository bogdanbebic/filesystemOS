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
	if (mode == FileOperations::WRITE)
	{
		if (!this->exists(filename))
		{
			this->files_.insert(filename);
			// TODO: disk operation
		}
		else
		{
			// TODO: delete contents of existing file from disk
		}
	}

	if (!this->exists(filename))
		return nullptr;

	this->opened_files_to_modes_map_[filename] = mode;

	// TODO: file opening
	return nullptr;
}

char KernelFS::delete_file(char* filename)
{
	// TODO: check open files
	if (this->exists(filename))
	{
		this->files_.erase(filename);
		// TODO: disk operation
	}

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
