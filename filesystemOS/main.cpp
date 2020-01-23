#include <iostream>

#include "part.h"

#include "FS.h"


int main()
{
	Partition partition{ (char*)"resources/p2.ini" };
	std::cout << "Hello, World!\n";
	FS::mount(&partition);

	FS::format();
	
	FS::unmount();
	return 0;
}
