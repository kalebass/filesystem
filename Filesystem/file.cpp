#include "file.h"

using std::istream;
using std::ostream;

File::File(BlockDevice& disk, int indexBlock, int size)	:
	disk{ disk },
	indexBlock_{ indexBlock },
	indexbuffer_{ nullptr },
	size_{ size },
	bsize_{ disk.blockSize() }
{}

void File::write(istream& input)
{
	if (!indexbuffer_) {
		indexbuffer_ = new char[bsize_];
		if (size_ > 0)
			disk.read(indexBlock_, indexbuffer_);
	}
	auto buffer = new char[bsize_];

	auto bpos = size_ % bsize_;
	size_ -= bpos;
	if (bpos > 0)
		disk.read(block(size_ / bsize_), buffer);
	else
		block(size_ / bsize_) = disk.nextFreeBlock();

	while (input.good()) {
		if (bpos == bsize_) {
			disk.write(block(size_ / bsize_), buffer);
			bpos = 0;
			size_ += bsize_;
			block(size_ / bsize_) = disk.nextFreeBlock();
		}
		input.get(buffer[bpos++]);
	}
	disk.write(block(size_ / bsize_), buffer);
	disk.write(indexBlock_, indexbuffer_);
	size_ += bpos - 1;
	delete[] buffer;
}

void File::read(ostream& output)
{
	if (size_ == 0) return;
	if (!indexbuffer_) {
		indexbuffer_ = new char[bsize_];
		disk.read(indexBlock_, indexbuffer_);
	}
	auto buffer = new char[bsize_];

	auto bytesLeft = size_;
	auto segment = 0;
	while (bytesLeft / bsize_ > 0) {
		disk.read(block(segment++), buffer);
		output.write(buffer, bsize_);
		bytesLeft -= bsize_;
	}
	if (bytesLeft > 0) {
		disk.read(block(segment), buffer);
		output.write(buffer, bytesLeft);
	}
	delete[] buffer;
}

void File::erase()
{
	if (size_ == 0) return;
	auto i = 0;
	while (i <= (size_ - 1) / bsize_) {
		disk.clear(block(i));
		++i;
	}
	size_ = 0;
	delete[] indexbuffer_;
	indexbuffer_ = nullptr;
}

void File::del()
{
	erase();
}

int& File::block(int segment)
{
	if (!indexbuffer_) {
		indexbuffer_ = new char[bsize_];
		if (size_ > 0)
			disk.read(indexBlock_, indexbuffer_);
	}
	return *(reinterpret_cast<int*>(indexbuffer_) + segment);
}
void File::blockUsage(ostream& output)
{
	output << 'i' << indexBlock_;
	if (size_ == 0) return;
	auto i = 0;
	while (i <= (size_ - 1) / bsize_) {
		output << ' ' << block(i);
		++i;
	}
}

int File::size() const
{
	return size_;
}
void File::setSize(int size)
{
	size_ = size;
}

int File::indexBlock() const
{
	return indexBlock_;
}

File::~File()
{
	delete[] indexbuffer_;
}
