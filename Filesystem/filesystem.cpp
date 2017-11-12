#include "filesystem.h"
#include <sstream>

using std::ostream;

Filesystem::Filesystem(BlockDevice& disk) :
    disk_{ disk },
    root_{ disk, 0, 0 },
    name_{ "root" }
{
    disk.use(0);
}
Filesystem::~Filesystem() {}

void Filesystem::buse(const path& p, ostream& output)
{
    dir(p).blockUsage(output);
}
void Filesystem::catr(ostream& output)
{
    root_.read(output);
}
void Filesystem::format(std::string name)
{
    disk_.clear();
    disk_.use(0);
    root_.setSize(0);
    root_.depopulate();
}
void Filesystem::fsinfo(ostream& output)
{
    output << "Disk size: " << disk_.blockCount() * disk_.blockSize();
    output << " byte.\n";
    output << "Block size: " << disk_.blockSize() << " byte.\n";
    output << disk_.usedBlockCount() << " out of " << disk_.blockCount();
    output << " blocks are in use.\n";
    disk_.listUsedBlocks(output);
}
void Filesystem::ls(const path& p, ostream& output)
{
    dir(p).ls(output);
}
void Filesystem::rename(path p, std::string newname)
{
    auto oldname = p.back();
    p.pop_back();
    dir(p).rename(oldname, newname);
}
void Filesystem::rm(path p)
{
    auto filename = p.back();
    p.pop_back();
    dir(p).delFile(filename);
    if (!empty(p)) {
        p.pop_back();
        dir(p).rewrite();
    }
}

void Filesystem::save()
{
    root_.rewrite();
    disk_.save(root_.size());
}
void Filesystem::load()
{
    root_.depopulate();
    root_.setSize(disk_.load());
}

void Filesystem::testDir(const path& p)
{
    dir(p);
}
void Filesystem::testFile(const path& p)
{
    file(p);
}
void Filesystem::createFile(path p, bool isDir)
{
    auto filename = p.back();
    p.pop_back();
    dir(p).createFile(filename, isDir);
    if (!empty(p)) {
        p.pop_back();
        dir(p).rewrite();
    }
}
void Filesystem::read(const path& p, ostream& output)
{
    file(p).read(output);
}
void Filesystem::write(path p, std::istream& input)
{
    auto filename = p.back();
    p.pop_back();
    dir(p).writeFile(filename, input);
}

Directory& Filesystem::dir(const path& p)
{
    auto workingDir = &root_;
    for (auto i = begin(p); i != end(p); ++i) {
        workingDir = &workingDir->subdir(*i);
    }
    return *workingDir;
}
File& Filesystem::file(path p)
{
    auto filename = p.back();
    p.pop_back();
    return dir(p).child(filename);
}

std::string Filesystem::getName() const
{
    return name_;
}
