#ifndef DIRECTORY_BLOCK_H
#define DIRECTORY_BLOCK_H

#include "t2fs.h"

typedef struct s_DirectoryBlock{
	t2fs_record entries[];
} typedef DirectoryBlock;

void DB_DirectoryBlock(DirectoryBlock* directoryBlock, byte* block);
t2fs_record* DB_findByName(DirectoryBlock* directoryBlock, char* name);
#endif
