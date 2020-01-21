#include <iostream>

#include "part.h"

#include "FS.h"

#include "IndexCluster.h"
#include "DirDataCluster.h"

// #define MY_TESTS

#ifdef MY_TESTS

char* f1 = (char*)"f1.txt", * f2 = (char*)"f2.txt", * f3 = (char*)"f3.txt";
char* f4 = (char*)"f4.txt", * f5 = (char*)"f5.txt", * f6 = (char*)"f6.txt";
char* f7 = (char*)"f7.txt", * f8 = (char*)"f8.txt", * f9 = (char*)"f9.txt";

void test_exists()
{
	std::cout << (int)FS::doesExist(f1) << '\n';
}

void test_open()
{
	std::cout << (int)FS::doesExist(f2) << '\n';
	FS::open(f2, 'w');
	std::cout << (int)FS::doesExist(f2) << '\n';
}

void test_delete()
{
	std::cout << (int)FS::doesExist(f3) << '\n';
	FS::open(f3, 'w');
	std::cout << (int)FS::doesExist(f3) << '\n';
	FS::deleteFile(f3);
	std::cout << (int)FS::doesExist(f3) << '\n';
}

void test_count_files()
{
	std::cout << FS::readRootDir() << '\n';
	FS::open(f4, 'w');
	std::cout << FS::readRootDir() << '\n';
	FS::open(f5, 'w');
	std::cout << FS::readRootDir() << '\n';
	FS::open(f6, 'w');
	std::cout << FS::readRootDir() << '\n';
}

void test_count_files_same_files()
{
	std::cout << FS::readRootDir() << '\n';
	FS::open(f7, 'w');
	std::cout << FS::readRootDir() << '\n';
	FS::open(f7, 'w');
	std::cout << FS::readRootDir() << '\n';
	FS::open(f7, 'w');
	std::cout << FS::readRootDir() << '\n';
}

void test_count_files_wrong_mode()
{
	std::cout << FS::readRootDir() << '\n';
	FS::open(f8, 'w');
	std::cout << FS::readRootDir() << '\n';
	FS::open(f9, 'r');
	std::cout << FS::readRootDir() << '\n';
	FS::open(f9, 'a');
	std::cout << FS::readRootDir() << '\n';
}

#include <vector>

class Test
{
public:
	Test(void (*callable)()) 
	{
		std::cout << "***** TEST START *****\n";
		callable();
		std::cout << "***** TEST END   *****\n";
	}

	Test(std::vector<void (*)()> vec) 
	{
		for (auto& callable : vec)
		{
			std::cout << "***** TEST START *****\n";
			callable();
			std::cout << "***** TEST END   *****\n";
		}
	}
};

#endif

int main()
{
	Partition partition{ (char*)"resources/p2.ini" };
	std::cout << "Hello, World!\n";

	IndexCluster index{ 0 };
	index.read_from_partition(&partition);
	std::cout << index.get_char_from_buffer(8)
		<< index.get_char_from_buffer(9)
		<< index.get_char_from_buffer(10)
		<< index.get_char_from_buffer(11)
		<< index.get_char_from_buffer(12);

	std::cout << std::endl;

	std::cout << index.get_cluster(1) << std::endl;
	std::cout << sizeof(dir_entry_t) << std::endl;

	DirDataCluster dir_data{ 0 };
	dir_data.read_from_partition(&partition);

	dir_data.set_dir_entry(0, { "imefajl", "ex" });
	dir_data.set_dir_entry(1, { "fajl1", {'e', 'x', 't' } });
	dir_data.set_dir_entry(3, { "fajl3", {'e', 'x', '3' } });
	dir_data.write_to_partition(&partition);

	DirDataCluster dir_data0{ 0 };
	dir_data0.read_from_partition(&partition);
	dir_entry_t entry = dir_data0.get_dir_entry(1);

	return 0;
}
