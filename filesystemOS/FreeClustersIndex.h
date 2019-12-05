#pragma once

#include <vector>

class Partition;

namespace kernel
{

	class FreeClustersIndex
	{
	public:
		void read_from_partition(Partition* partition);
		void write_to_partition(Partition* partition) const;

		// member access:
		// without bounds checking
		std::vector<bool>::reference operator[] (size_t index);
		std::vector<bool>::const_reference operator[] (size_t index) const;
		// with bounds checking
		std::vector<bool>::reference at(size_t index);
		std::vector<bool>::const_reference at(size_t index) const;

	private:
		void read_free_clusters_from_buffer(char* buffer);
		void write_free_clusters_to_buffer(char* buffer) const;

		void set_bit(char* buffer, size_t index_of_bit) const;
		void clear_bit(char* buffer, size_t index_of_bit) const;
		bool read_bit(char* buffer, size_t index_of_bit) const;

		static const size_t partition_cluster_size = 2048;
		std::vector<bool> free_clusters_;
	};

}
