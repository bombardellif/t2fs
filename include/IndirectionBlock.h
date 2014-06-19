#ifndef INDIRECTION_BLOCK_H
#define INDIRECTION_BLOCK_H

#include "t2fs.h"
#include "DirectoryBlock.h"

#define IB_INVALID_ARGUMENT -1
#define IB_CANT_ALLOCATE -2
#define IB_SUCCESS 0

typedef struct s_IndirectionBlock{
	DWORD* dataPtr;
} IndirectionBlock;

void IB_IndirectionBlock(IndirectionBlock* this, BYTE* block);
Record* IB_find(IndirectionBlock* this, char* name, int level, BYTE* block, DWORD* blockAddress, Record*(*find)(DirectoryBlock*, char* param));
int IB_allocateNewDirectoryBlock(IndirectionBlock* this, int level, BYTE* block, DWORD* blockAddress);
int IB_findBlockByNumber(IndirectionBlock* this, int level, DWORD number, BYTE* block, DWORD* blockAddress);

#endif
