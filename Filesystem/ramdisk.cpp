#include "ramdisk.h"
#include <cstring>

RamDisk::RamDisk(int blockSize, int blockCount)    :
    BlockDevice{ blockSize, blockCount }
{
    storage_ = new char[blockSize_ * blockCount_];
}

RamDisk::~RamDisk()
{
    delete[] storage_;
}

void RamDisk::read(int index, char* buffer) const
{
    if (index < 0 || index > blockCount_ - 1) {
        throw "Block out of range";
    }
    auto block = storage_ + index * blockSize_;
    std::memcpy(buffer, block, blockSize_);
}

void RamDisk::write(int index, const char* data)
{
    if (index < 0 || index > blockCount_ - 1) {
        throw "Block out of range";
    }
    auto block = storage_ + index * blockSize_;
    usage.set(index);
    std::memcpy(block, data, blockSize_);
}
