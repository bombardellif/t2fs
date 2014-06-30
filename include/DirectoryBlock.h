#ifndef DIRECTORY_BLOCK_H
#define DIRECTORY_BLOCK_H

#include "t2fs.h"

typedef struct s_DirectoryBlock{
	Record* entries;
} DirectoryBlock;

void DB_DirectoryBlock(DirectoryBlock* this, BYTE* block);
Record* DB_findByName(const DirectoryBlock* const this, const char* name);
Record* DB_findEmpty(const DirectoryBlock* const this, const char* const notUsed);

//Extra for utilities
void DB_forEachEntry(const DirectoryBlock* const this, void(*callback)(const Record* const));

#endif
