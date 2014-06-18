#include "IndirectionBlock.h"


void IB_IndirectionBlock(IndirectionBlock* this, BYTE* block)
{
}

t2fs_record* IB_find(IndirectionBlock* this, char* name, int level, BYTE* block, DWORD* blockAddress)
{
	return 0;
}

int IB_allocateNewDirectoryBlock(IndirectionBlock* this, int level, BYTE* block, DWORD* blockAddress)
{
	return 0;
}
