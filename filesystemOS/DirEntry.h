#pragma once

#include <stdint.h>

constexpr size_t NAME_SIZE = 8;
constexpr size_t EXT_SIZE = 3;
constexpr size_t KERNEL_USE_SIZE_PADDING = 4;

typedef struct DirEntry {
	char name[NAME_SIZE] = {};
	char extension[EXT_SIZE] = {};
	char dummy = 0;
	uint32_t first_level_index_of_file = 0;
	uint32_t file_size = 0;
	uint32_t cluster_number = 0;
	uint32_t offset_in_cluster = 0;
	char kernel_use[KERNEL_USE_SIZE_PADDING] = {};
} dir_entry_t;
