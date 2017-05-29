#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <string>
#include <vector>
#include "blockdevice.h"
#include "directory.h"

using path = std::vector<std::string>;

class Filesystem
{
public:
	Filesystem(BlockDevice&);
	~Filesystem();
	
	void buse(const path&, std::ostream&);
	void catr(std::ostream&);
	void format(std::string name);
	void fsinfo(std::ostream&);
	void ls(const path&, std::ostream&);
	void rename(path, std::string newname);
	void rm(path);
		 
	void save();
	void load();
		 
	void testDir(const path&);
	void testFile(const path&);
	void createFile(path, bool isDir = false);
	void read(const path&, std::ostream&);
	void write(path, std::istream&);

	std::string getName() const;

private:
	BlockDevice &disk_;
	Directory root_;
	std::string name_;
	Directory& dir(const path&);
	File& file(path);
};

#endif
