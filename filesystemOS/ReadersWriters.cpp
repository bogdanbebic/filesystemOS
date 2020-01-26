// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppClangTidyPerformanceUnnecessaryValueParam
// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
#include "ReadersWriters.h"

ReadersWriters::ReadersWriters()
{
	this->mutex_ = CreateSemaphore(NULL, 1, 32, NULL);
}

void ReadersWriters::acquire(value_type value, bool is_reader)
{
	if (is_reader)
		this->reader_acquire(value);
	else
		this->writer_acquire(value);
}

void ReadersWriters::release(value_type value, bool is_reader)
{
	if (is_reader)
		this->reader_release(value);
	else
		this->writer_release(value);
}

void ReadersWriters::reader_acquire(value_type value)
{
	WaitForSingleObject(this->mutex_, INFINITE);
	if (!this->exists_reader_writer(value))
		this->register_reader_writer(value);

	this->number_of_occurrences_[value]++;
	if (this->number_of_occurrences_[value] == 1)
		WaitForSingleObject(this->sem_map_[value], INFINITE);

	ReleaseSemaphore(this->mutex_, 1, NULL);
}

void ReadersWriters::writer_acquire(value_type value)
{
	WaitForSingleObject(this->mutex_, INFINITE);
	if (!this->exists_reader_writer(value))
		this->register_reader_writer(value);
	
	WaitForSingleObject(this->sem_map_[value], INFINITE);
	ReleaseSemaphore(this->mutex_, 1, NULL);
}

void ReadersWriters::reader_release(value_type value)
{
	WaitForSingleObject(this->mutex_, INFINITE);

	this->number_of_occurrences_[value]--;
	if (this->number_of_occurrences_[value] == 0)
		ReleaseSemaphore(this->sem_map_[value], 1, NULL);
	
	ReleaseSemaphore(this->mutex_, 1, NULL);
}

void ReadersWriters::writer_release(value_type value)
{
	ReleaseSemaphore(this->sem_map_[value], 1, NULL);
}

bool ReadersWriters::exists_reader_writer(value_type value)
{
	return this->number_of_occurrences_.find(value) != this->number_of_occurrences_.end();
}

void ReadersWriters::register_reader_writer(value_type value)
{
	this->number_of_occurrences_[value] = 0;
	this->sem_map_[value] = CreateSemaphore(NULL, 1, 32, NULL);
}

void ReadersWriters::unregister_reader_writer(value_type value)
{
	this->number_of_occurrences_.erase(value);
	this->sem_map_.erase(value);
}
