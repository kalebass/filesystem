#ifndef BLOCKDEVICE_H
#define BLOCKDEVICE_H
#include <bitset>
#include <iostream>

class BlockDevice
{
public:
    BlockDevice(int blockSize, int blockCount);
    virtual ~BlockDevice();

    virtual void write(int index, const char* data) = 0;
    virtual void read(int index, char* buffer) const = 0;
    void use(int index);
    void clear(int index);
    void clear();

    void save(int rootSize);
    int load();

    int blockCount() const;
    int blockSize() const;
    int usedBlockCount() const;
    int nextFreeBlock() const;
    void listUsedBlocks(std::ostream&) const;

protected:
    const int blockSize_;
    const int blockCount_;
    std::bitset<(1 << 16)> usage;
};

#endif
