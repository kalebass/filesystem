#ifndef FILE_H
#define FILE_H
#include <iostream>
#include <string>
#include "blockdevice.h"

class File
{
public:
	File(BlockDevice&, int indexBlock, int size);
	virtual ~File();

	void write(std::istream&);
	void read(std::ostream&);
	void erase();
	virtual void del();

	virtual void blockUsage(std::ostream&);

	int size() const;
	void setSize(int);
	int indexBlock() const;

protected:
	BlockDevice& disk;

	int indexBlock_;
	int size_;
	int bsize_;

	char* indexbuffer_;

	int& block(int segment);
};

#endif
