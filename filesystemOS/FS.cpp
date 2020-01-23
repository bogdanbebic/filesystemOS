// ReSharper disable CppInconsistentNaming
#include "FS.h"

#include "KernelFS.h"


KernelFS* FS::myImpl = KernelFS::get_instance();

FS::~FS()
{
	// empty body
}

char FS::mount(Partition* partition)
{
	return myImpl->mount(partition);
}

char FS::unmount()
{
	return myImpl->unmount();
}

char FS::format()
{
	return myImpl->format();
}

FileCnt FS::readRootDir()
{
	return myImpl->number_of_files();
}

char FS::doesExist(char* fname)
{
	return myImpl->exists(fname);
}

// ReSharper disable once CppParameterMayBeConst
File* FS::open(char* fname, char mode)
{
	return myImpl->open(fname, mode);
}

char FS::deleteFile(char* fname)
{
	return myImpl->delete_file(fname);
}

FS::FS()
{
	// empty body
}
