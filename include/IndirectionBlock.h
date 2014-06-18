#ifndef INDIRECTION_BLOCK_H
#define INDIRECTION_BLOCK_H

#include "t2fs.h"

typedef struct s_IndirectionBlock{
	dword dataPtr[];
} IndirectionBlock;

void IB_IndirectionBlock(IndirectionBlock* this, BYTE* block);
t2fs_record* IB_find(IndirectionBlock* this, char* name, int level, BYTE* block, DWORD* blockAddress);
int IB_allocateNewDirectoryBlock(IndirectionBlock* this, int level, BYTE* block, DWORD* blockAddress);

#endif
