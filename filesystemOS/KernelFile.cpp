#include "KernelFile.h"

char KernelFile::write(bytes_cnt_t bytes_cnt, char* buffer)
{
	// TODO: implement
	return 1;
}

bytes_cnt_t KernelFile::read(bytes_cnt_t bytes_cnt, char* buffer)
{
	// TODO: implement
	return 0;
}

char KernelFile::seek(bytes_cnt_t new_position)
{
	if (new_position > this->size_)
		return 0;

	this->current_position_ = new_position;
	return 1;
}

bytes_cnt_t KernelFile::get_current_position() const
{
	return this->current_position_;
}

/**
 * \brief Checks if current position is at end of file
 * \return 0 - no, 1 - error, 2 - yes
 */
char KernelFile::eof() const
{
	return this->current_position_ == this->size_ ? 2 : 0;
}

bytes_cnt_t KernelFile::get_file_size() const
{
	return this->size_;
}

char KernelFile::truncate()
{
	// TODO: implement
	return 1;
}

void KernelFile::set_dir_entry(dir_entry_t dir_entry)
{
	this->dir_entry_ = dir_entry;
}
