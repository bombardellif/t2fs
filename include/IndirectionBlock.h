#ifndef INDIRECTION_BLOCK_H
#define INDIRECTION_BLOCK_H

#include "t2fs.h"
#define IB_NULL_BLOCK_POINTER 0xFFFFFFFF

typedef struct s_IndirectionBlock{
	DWORD dataPtr[];
} IndirectionBlock;

void IB_IndirectionBlock(IndirectionBlock* this, BYTE* block);
Record* IB_find(IndirectionBlock* this, char* name, int level, BYTE* block, DWORD* blockAddress);
int IB_allocateNewDirectoryBlock(IndirectionBlock* this, int level, BYTE* block, DWORD* blockAddress);

#endif
