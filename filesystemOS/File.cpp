#include "File.h"

#include "KernelFile.h"

File::~File()
{
	delete this->myImpl;
}

char File::write(BytesCnt bytes_cnt, char* buffer)
{
	return this->myImpl->write(bytes_cnt, buffer);
}

BytesCnt File::read(BytesCnt bytes_cnt, char* buffer)
{
	return this->myImpl->read(bytes_cnt, buffer);
}

char File::seek(BytesCnt bytes_cnt)
{
	return this->myImpl->seek(bytes_cnt);
}

BytesCnt File::filePos()
{
	return this->myImpl->get_current_position();
}

char File::eof()
{
	return this->myImpl->eof();
}

BytesCnt File::getFileSize()
{
	return this->myImpl->get_file_size();
}

char File::truncate()
{
	return this->myImpl->truncate();
}

File::File()
{
	this->myImpl = new KernelFile{};
}
