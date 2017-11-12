#ifndef RAMDISK_H
#define RAMDISK_H
#include "blockdevice.h"

class RamDisk : public BlockDevice
{
public:
    RamDisk(int blockSize, int blockCount);
    ~RamDisk();

    void write(int index, const char* data) override;
    void read(int index, char* buffer) const override;

private:
    char* storage_;
};

#endif
