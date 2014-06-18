#ifndef INDIRECTION_BLOCK_H
#define INDIRECTION_BLOCK_H

#include "t2fs.h"

typedef struct s_IndirectionBlock{
	dword dataPtr[];
} IndirectionBlock;

void IB_IndirectionBlock(IndirectionBlock* indirectionBlock, byte* block);
t2fs_record* IB_find(IndirectionBlock* indirectionBlock, char* name, int level, byte* block, dword* blockAddress);
int IB_allocateNewDirectoryBlock(IndirectionBlock* indirectionBlock, int level, byte* block, dword* blockAddress);

#endif
