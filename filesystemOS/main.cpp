#include <iostream>

#include "part.h"

#include "FS.h"


int main()
{
	Partition partition{ (char*)"resources/p2.ini" };
	std::cout << "Hello, World!\n";
	FS::mount(&partition);

	// FS::format();
	std::cout << "File cnt: " << FS::readRootDir() << std::endl;
	std::cout << "Does exist: " << (bool)FS::doesExist((char*)"/fajl2.txt") << std::endl;
	std::cout << "Does exist: " << (bool)FS::doesExist((char*)"/fajl4.txt") << std::endl;

	FS::deleteFile((char*)"/fajl2.txt");
	std::cout << "Deleted /fajl2.txt\n";
	std::cout << "Does exist: " << (bool)FS::doesExist((char*)"/fajl2.txt") << std::endl;
	FS::open((char*)"/file1.txt", 'w');
	FS::open((char*)"/file2.txt", 'w');
	FS::open((char*)"/file3.txt", 'w');

	std::cout << "Does exist: " << (bool)FS::doesExist((char*)"/file3.txt") << std::endl;
	
	FS::unmount();
	return 0;
}
