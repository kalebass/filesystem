#ifndef DIRECTORY_H
#define DIRECTORY_H
#include <unordered_map>
#include <string>
#include "file.h"

class Directory : public File
{
public:
	Directory(BlockDevice&, int indexBlock, int size);
	~Directory();

	void createFile(std::string name, bool isDir = false);
	void writeFile(std::string name, std::istream&);
	void delFile(std::string&);
	void rename(std::string&, std::string&);

	void del() override;
	void ls(std::ostream&);
	void blockUsage(std::ostream&) override;

	File& child(std::string);
	Directory& subdir(std::string);

	void populate();
	void depopulate();
	void rewrite();

private:
	std::unordered_map<std::string, File> children_;
	std::unordered_map<std::string, Directory> subdirs_;
	bool populated_;
};

#endif
