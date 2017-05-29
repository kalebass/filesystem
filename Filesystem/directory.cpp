#include "directory.h"
#include <iomanip>
#include <sstream>

using namespace std;

Directory::Directory(BlockDevice& volume, int indexBlock, int size)	:
	File{ volume, indexBlock, size },
	populated_{ false }
{}

void Directory::createFile(string name, bool isDir)
{
	populate();
	stringstream dirContents;
	auto fileSize = 0;
	auto fileIndex = disk.nextFreeBlock();
	disk.use(fileIndex);
	dirContents << setw(30) <<  name << ' ';
	dirContents << setw(10) << fileIndex  << ' ';
	dirContents << setw(10) << fileSize << ' ';
	dirContents << (isDir ? 'd' : 'f') << '\n';
	write(dirContents);
	if (isDir) {
		subdirs_.emplace(name, Directory(disk, fileIndex, fileSize));
	} else {
		children_.emplace(name, File(disk, fileIndex, fileSize));
	}
}
void Directory::writeFile(string name, istream& input)
{
	child(name).write(input);
	rewrite();
}
void Directory::delFile(string& name)
{
	populate();
	unordered_map<string, File>::iterator it;
	unordered_map<string, Directory>::iterator itd;
	if ((it = children_.find(name)) != end(children_)) {
		it->second.del();
		disk.clear(it->second.indexBlock());
		children_.erase(it);
		rewrite();
	} else if ((itd = subdirs_.find(name)) != end(subdirs_)) {
		itd->second.del();
		disk.clear(itd->second.indexBlock());
		subdirs_.erase(itd);
		rewrite();
	} else {
		throw "No such file or directory.";
	}
}

void Directory::del()
{
	populate();
	for (auto& pair : children_) {
		pair.second.del();
		disk.clear(pair.second.indexBlock());
	}
	for (auto& pair : subdirs_) {
		pair.second.del();
		disk.clear(pair.second.indexBlock());
	}
	children_.clear();
	subdirs_.clear();
	erase();
}
void Directory::ls(ostream& out)
{
	populate();
	out << '\n';
	if (empty(children_) && empty(subdirs_)) {
		out << "Directory is empty.";
		return;
	}
	out << left << setw(30) << "Name";
	out << right << setw(10) << "Size" << '\n';
	for (const auto& pair : subdirs_) {
		out << left << setw(30) << pair.first;
		out << right << setw(10) << pair.second.size();
		out << " <dir>\n";
	}
	for (const auto& pair : children_) {
		out << left << setw(30) << pair.first;
		out << right << setw(10) << pair.second.size() << '\n';
	}
}
void Directory::blockUsage(ostream& out)
{
	File::blockUsage(out);
	populate();
	for (auto& pair : subdirs_) {
		out << '\n' << pair.first << ": ";
		pair.second.blockUsage(out);
	}
	for (auto& pair : children_) {
		out << '\n' << pair.first << ": ";
		pair.second.blockUsage(out);
	}
}

void Directory::rename(string& name, string& newname)
{
	populate();
	unordered_map<string, File>::iterator it;
	unordered_map<string, Directory>::iterator itd;
	int indexBlock;
	int size;
	if ((it = children_.find(name)) != end(children_)) {
		indexBlock = it->second.indexBlock();
		size = std::size(it->second);
		children_.erase(it);
		children_.emplace(newname, File(disk, indexBlock, size));
		rewrite();
	} else if ((itd = subdirs_.find(name)) != end(subdirs_)) {
		indexBlock = itd->second.indexBlock();
		size = std::size(itd->second);
		subdirs_.erase(itd);
		subdirs_.emplace(newname, Directory(disk, indexBlock, size));
		rewrite();
	} else {
		throw "No such file or directory.";
	}
}

File& Directory::child(string filename)
{
	populate();
	auto it = children_.find(filename);
	if (it == end(children_)) throw "File does not exist.";
	return it->second;
}
Directory& Directory::subdir(string name)
{
	populate();
	auto it = subdirs_.find(name);
	if (it == end(subdirs_)) throw "Directory does not exist.";
	return it->second;
}

void Directory::populate()
{
	if (populated_) return;
	stringstream buffer;
	read(buffer);
	string name;
	auto fileIndex = 0;
	auto fileSize = 0;
	auto type = '\0';
	while (buffer.good()) {
		buffer >> name >> fileIndex >> fileSize >> type;
		if (type == 'd') {
			subdirs_.emplace(name, Directory(disk, fileIndex, fileSize));
		} else if (type == 'f') {
			children_.emplace(name, File(disk, fileIndex, fileSize));
		}
	}
	populated_ = true;
}
void Directory::depopulate()
{
	children_.clear();
	subdirs_.clear();
	populated_ = false;
}
void Directory::rewrite()
{
	erase();
	if (empty(children_) && empty(subdirs_)) return;
	stringstream buffer;
	for (const auto& pair : subdirs_) {
		const Directory& d = pair.second;
		buffer << setw(30) << pair.first << ' ';
		buffer << setw(10) << d.indexBlock() << ' ';
		buffer << setw(10) << d.size() << " d\n";
	}
	for (const auto& pair : children_) {
		const File& f = pair.second;
		buffer << setw(30) << pair.first << ' ';
		buffer << setw(10) << f.indexBlock() << ' ';
		buffer << setw(10) << f.size() << " f\n";
	}
	write(buffer);
}

Directory::~Directory() {}
