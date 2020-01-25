#pragma once

#include "FS.h"
#include "DirEntry.h"

using bytes_cnt_t = BytesCnt;

class KernelFile
{
public:
	char write(bytes_cnt_t bytes_cnt, char* buffer);
	bytes_cnt_t read(bytes_cnt_t bytes_cnt, char* buffer);
	
	char seek(bytes_cnt_t new_position);

	bytes_cnt_t get_current_position() const;

	char eof() const;

	bytes_cnt_t get_file_size() const;

	char truncate();


	void set_dir_entry(dir_entry_t dir_entry);
private:
	bytes_cnt_t current_position_ = 0;
	bytes_cnt_t size_ = 0;
	
	dir_entry_t dir_entry_ = {};
	
};

