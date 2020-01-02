#include <iostream>

#include "part.h"

#include "FS.h"

#define MY_TESTS

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
	Partition partition{ (char*)"resources/p1.ini" };
	std::cout << "Hello, World!\n";
	// Test{ {test_exists, test_open, test_delete, test_count_files, test_count_files_same_files, test_count_files_wrong_mode} };
	Test{ []() {
		char* file = (char*)"file.txt";
		FS::open(file, 'w');
		FS::open(file, 'r');
		FS::open(file, 'a');
	} };
	return 0;
}
