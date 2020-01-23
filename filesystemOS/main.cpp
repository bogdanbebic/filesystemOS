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
	std::cout << "Does exist: " << (bool)FS::doesExist((char*)"fajl2   txt") << std::endl;
	std::cout << "Does exist: " << (bool)FS::doesExist((char*)"fajl24  txt") << std::endl;

	FS::unmount();
	return 0;
}
