#pragma once
#include <map>
#include <string>

#include <Windows.h>

class ReadersWriters
{
public:
	using value_type = std::string;

	ReadersWriters();

	void acquire(value_type value, bool is_reader);
	void release(value_type value, bool is_reader);
	
private:
	void reader_acquire(value_type value);
	void writer_acquire(value_type value);

	void reader_release(value_type value);
	void writer_release(value_type value);

	bool exists_reader_writer(value_type value);
	
	void register_reader_writer(value_type value);
	void unregister_reader_writer(value_type value);
	
	std::map<value_type, size_t> number_of_occurrences_;
	std::map<value_type, HANDLE> sem_map_;
	HANDLE mutex_;
};

