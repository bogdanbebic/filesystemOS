#include "FreeClustersIndex.h"

// std::min
#include <algorithm>

void kernel::FreeClustersIndex::read_from_partition(Partition* partition)
{
	char buffer[kernel::FreeClustersIndex::partition_cluster_size] = { 0 };
	// partition->read(FREE_CLUSTERS_INDEX_NO, buffer);
	this->read_free_clusters_from_buffer(buffer);
}

void kernel::FreeClustersIndex::write_to_partition(Partition* partition) const
{
	char buffer[kernel::FreeClustersIndex::partition_cluster_size] = { 0 };
	this->write_free_clusters_to_buffer(buffer);
	// partition->write(FREE_CLUSTERS_INDEX_NO, buffer);
}

std::vector<bool>::reference kernel::FreeClustersIndex::operator[](size_t index)
{
	return this->free_clusters_[index];
}

std::vector<bool>::const_reference kernel::FreeClustersIndex::operator[](size_t index) const
{
	return this->free_clusters_[index];
}

std::vector<bool>::reference kernel::FreeClustersIndex::at(size_t index)
{
	return this->free_clusters_.at(index);
}

std::vector<bool>::const_reference kernel::FreeClustersIndex::at(size_t index) const
{
	return this->free_clusters_.at(index);
}

void kernel::FreeClustersIndex::read_free_clusters_from_buffer(char* buffer)
{
	if (this->free_clusters_.size() < partition_cluster_size * CHAR_BIT)
		this->free_clusters_.resize(partition_cluster_size * CHAR_BIT);
	for (size_t i = 0; i < partition_cluster_size * CHAR_BIT; i++)
		this->free_clusters_[i] = this->read_bit(buffer, i);
}

void kernel::FreeClustersIndex::write_free_clusters_to_buffer(char* buffer) const
{
	for (size_t i = 0; i < std::min(partition_cluster_size * CHAR_BIT, this->free_clusters_.size()); i++)
		if (this->free_clusters_[i])
			this->set_bit(buffer, i);
		else
			this->clear_bit(buffer, i);
}

void kernel::FreeClustersIndex::set_bit(char* buffer, size_t index_of_bit) const
{
	size_t index_of_byte = index_of_bit / CHAR_BIT;
	size_t index_of_bit_in_byte = index_of_bit % CHAR_BIT;
	buffer[index_of_byte] |= 1 << index_of_bit_in_byte;
}

void kernel::FreeClustersIndex::clear_bit(char* buffer, size_t index_of_bit) const
{
	size_t index_of_byte = index_of_bit / CHAR_BIT;
	size_t index_of_bit_in_byte = index_of_bit % CHAR_BIT;
	buffer[index_of_byte] &= ~(1 << index_of_bit_in_byte);
}

bool kernel::FreeClustersIndex::read_bit(char* buffer, size_t index_of_bit) const
{
	size_t index_of_byte = index_of_bit / CHAR_BIT;
	size_t index_of_bit_in_byte = index_of_bit % CHAR_BIT;
	return (buffer[index_of_byte] & 1 << index_of_bit_in_byte) != 0;
}
