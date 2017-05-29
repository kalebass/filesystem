#include "shell.h"
#include <fstream>
#include <sstream>

using std::string;

std::vector<string> splitString(string str, char delimiter)
{
	std::vector<string> list;
	auto startpos = 0ull;
	auto findpos = 0ull;
	while (findpos != string::npos) {
		findpos = str.find(delimiter, startpos);
		list.push_back(str.substr(startpos, findpos - startpos));
		startpos = findpos + 1;
	}
	return list;
}

Shell::Shell(Filesystem& fs, std::istream& in, std::ostream& out) :
	input{ &in },
	output{ &out },
	fs{ fs }
{}

void Shell::start()
{
	*output << "AFS shell\n";
	string line;
	while (line != "quit" && *input) {
		*output << '\n';
		pwd();
		*output << '>';
		getline(*input, line);
		cmdLine_ = splitString(line, ' ');
		try {
			interpret();
		} catch (char* msg) {
			*output << msg << '\n';
		}
	}
}

void Shell::interpret()
{
	const auto& cmd = cmdLine_.front();
	auto argc = cmdLine_.size();
	if (cmd == "append") {
		if (argc == 3) append();
		else *output << "Usage: append <source> <destination>";
	} else if (cmd == "buse") {
		if (argc == 1) fs.buse(currentPath_, *output);
		else if (argc == 2) fs.buse(combine(cmdLine_.at(1)), *output);
	} else if (cmd == "cat") {
		if (argc == 2) fs.read(combine(cmdLine_.at(1)), *output);
		else *output << "Usage: cat <file>";
	} else if (cmd == "catr") {
		fs.catr(*output);
	} else if (cmd == "cd") {
		if (argc == 2) {
			std::vector<string> newpath = combine(cmdLine_.at(1));
			fs.testDir(newpath);
			currentPath_ = newpath;
		}
		else *output << "Usage: cd <path>";
	} else if (cmd == "copy") {
		if (argc == 3) copy();
		else *output << "Usage: copy <source> <destination>";
	} else if (cmd == "create") {
		if (argc == 2) create();
		else *output << "Usage: create <file>";
	} else if (cmd == "format") {
		if (argc == 1) {
			fs.format("fs");
			currentPath_.clear();
		}
		else *output << "Usage: format";
	} else if (cmd == "fsinfo") {
		if (argc == 1) fs.fsinfo(*output);
		else *output << "Usage: fsinfo";
	} else if (cmd == "help") {
		printFile("help.txt");
	} else if (cmd == "import") {
		if (argc == 3) import();
		else *output << "Usage: import <real-file source> <destination>";
	} else if (cmd == "ls") {
		if (argc == 1) fs.ls(currentPath_, *output);
		else if (argc == 2) fs.ls(combine(cmdLine_.at(1)), *output);
		else *output << "Usage: ls [<path>]";
	} else if (cmd == "mkdir") {
		if (argc == 2) fs.createFile(combine(cmdLine_.at(1)), true);
		else *output << "Usage: mkdir <directory name>";
	} else if (cmd == "pwd") {
		if (argc == 1) pwd();
		else *output << "Usage: pwd";
	} else if (cmd == "quit") {
		return;
	} else if (cmd == "read") {
		if (argc == 1) {
			fs.load();
			currentPath_.clear();
		}
		else *output << "Usage: read";
	} else if (cmd == "rename") {
		if (argc == 3) fs.rename(combine(cmdLine_.at(1)), cmdLine_.at(2));
		else *output << "Usage: rename <old file> <new file>";
	} else if (cmd == "rm") {
		if (argc == 2) fs.rm(combine(cmdLine_.at(1)));
		else *output << "Usage: rm <file>";
	} else if (cmd == "save") {
		if (argc == 1) fs.save();
		else *output << "Usage: save";
	} else {
		*output << "Unknown command";
	}
	*output << '\n';
}

void Shell::append()
{
	auto source = combine(cmdLine_.at(1));
	auto dest = combine(cmdLine_.at(2));
	std::stringstream buffer;
	fs.read(source, buffer);
	fs.write(dest, buffer);
}
void Shell::copy()
{
	fs.createFile(combine(cmdLine_.at(2)));
	append();
}
void Shell::create()
{
	auto p = combine(cmdLine_.at(1));
	fs.createFile(p);
	std::stringstream buffer;
	string line;
	getline(*input, line);
	while (line != "") {
		buffer << line << '\n';
		getline(*input, line);
	}
	fs.write(p, buffer);
}
void Shell::import() const
{
	std::ifstream realfile(cmdLine_.at(1));
	if (!realfile) {
		*output << "Real file not found";
		return;
	}
	auto path = combine(cmdLine_.at(2));
	auto filename = path.back();
	fs.createFile(path);
	fs.write(path, realfile);
}
void Shell::printFile(string path) const
{
	std::ifstream file(path);
	if (!file) {
		*output << "Not found\n";
		return;
	}
	string line;
	while (file) {
		std::getline(file, line);
		*output << line << '\n';
	}
}
void Shell::pwd() const
{
	*output << fs.getName() << '/';
	for (const auto& dirName : currentPath_) {
		*output << dirName << '/';
	}
}

std::vector<string> Shell::combine(const string& str) const
{
	auto fullpath = currentPath_;
	auto relpath = splitString(str, '/');
	for (auto& dirName : relpath) {
		if (dirName == ".") continue;
		else if (dirName == "..") {
			if (empty(fullpath)) throw "Incorrect path.";
			fullpath.pop_back();
		} else {
			fullpath.push_back(dirName);
		}
	}
	return fullpath;
}

Shell::~Shell() {}
