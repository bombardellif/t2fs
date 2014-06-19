#ifndef DIRECTORY_BLOCK_H
#define DIRECTORY_BLOCK_H

#include "t2fs.h"

typedef struct s_DirectoryBlock{
	Record* entries;
} DirectoryBlock;

void DB_DirectoryBlock(DirectoryBlock* this, BYTE* block);
Record* DB_findByName(DirectoryBlock* this, char* name);
Record* DB_findEmpty(const DirectoryBlock* const this, const char* const notUsed);
#endif
