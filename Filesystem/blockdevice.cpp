#include "blockdevice.h"
#include <fstream>

BlockDevice::BlockDevice(int blockSize, int blockCount)	:
	blockSize_{ blockSize },
	blockCount_{ blockCount }
{}

BlockDevice::~BlockDevice() {}

int BlockDevice::nextFreeBlock() const
{
	auto size = blockCount_ * blockSize_;
	for (auto i = 0; i < size; ++i) {
		if (!usage.test(i)) {
			return i;
		}
	}
	throw "Disk full";
}

void BlockDevice::use(int index)
{
	usage.set(index);
}

void BlockDevice::clear(int index)
{
	usage.reset(index);
}
void BlockDevice::clear()
{
	usage.reset();
}

int BlockDevice::usedBlockCount() const
{
	return static_cast<int>(usage.count());
}
int BlockDevice::blockSize() const
{
	return blockSize_;
}
int BlockDevice::blockCount() const
{
	return blockCount_;
}
void BlockDevice::listUsedBlocks(std::ostream& out) const
{
	auto nBlocks = usage.count();
	for (auto i = 0, found = 0; found < nBlocks; ++i) {
		if (usage.test(i)) {
			out << i << ' ';
			++found;
		}
	}
}

void BlockDevice::save(int rootSize)
{
	std::ofstream data("image");
	std::ofstream meta("image-m");
	auto buffer = new char[blockSize_];
	auto nBlocks = usage.count();
	meta << rootSize << ' ';
	for (auto i = 0, found = 0; found < nBlocks; ++i) {
		if (usage.test(i)) {
			++found;
			meta << i << ' ';
			read(i, buffer);
			data.write(buffer, blockSize_);
		}
	}
	delete[] buffer;
}

int BlockDevice::load()
{
	std::ifstream data("image");
	std::ifstream meta("image-m");
	clear();
	auto buffer = new char[blockSize_];
	auto block = 0;
	auto rootSize = 0;
	meta >> rootSize;
	while (meta >> block) {
		data.read(buffer, blockSize_);
		write(block, buffer);
	}
	delete[] buffer;
	return rootSize;
}
