#include <clocale>
#include "shell.h"
#include "ramdisk.h"
#include "filesystem.h"

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::setlocale(LC_ALL, "Swedish");
	RamDisk disk{ 1024, 500 };
	Filesystem afs{ disk };
	Shell s1{ afs, std::cin, std::cout };
	s1.start();
}
