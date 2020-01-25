#include "BitVectorCluster.h"

// CHAR_BIT
#include <climits>
// std::memset
#include <cstring>

BitVectorCluster::BitVectorCluster(cluster_number_t cluster_number)
	: Cluster(cluster_number)
{
	// empty body
}

void BitVectorCluster::set_bit(size_t index, bool value)
{
	this->dirty_ = true;
	if (value)
		this->set_bit(index);
	else
		this->clear_bit(index);
}

void BitVectorCluster::set_bit(size_t index)
{
	size_t index_of_byte = index / CHAR_BIT;
	size_t index_of_bit_in_byte = index % CHAR_BIT;
	this->buffer_[index_of_byte] |= 1 << index_of_bit_in_byte;
}

void BitVectorCluster::clear_bit(size_t index)
{
	size_t index_of_byte = index / CHAR_BIT;
	size_t index_of_bit_in_byte = index % CHAR_BIT;
	this->buffer_[index_of_byte] &= ~(1 << index_of_bit_in_byte);
}

bool BitVectorCluster::get_bit(size_t index) const
{
	size_t index_of_byte = index / CHAR_BIT;
	size_t index_of_bit_in_byte = index % CHAR_BIT;
	return (this->buffer_[index_of_byte] & 1 << index_of_bit_in_byte) != 0;
}

void BitVectorCluster::format()
{
	this->dirty_ = true;
	std::memset(this->buffer_, -1, sizeof this->buffer_);
}
