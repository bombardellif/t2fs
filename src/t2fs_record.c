#include "t2fs_record.h"


void TR_t2fs_record(t2fs_record* this, BYTE typeVal, char* name, DWORD blocksFileSize, DWORD bytesFileSize)
{
}

t2fs_record* TR_find(t2fs_record* this, FilePath filepath, OpenRecord* openRecord, BYTE* block, BYTE blockTrace[], DWORD* recordPointerTrace[], DWORD blockAddress[])
{
	return 0;
}

int TR_addRecord(t2fs_record* this, t2fs_record newRecord, OpenRecord* newOpenRecord)
{
	return 0;
}

void TR_freeBlocks(t2fs_record* this)
{
}

t2fs_record* TR_findEmptyEntry(t2fs_record* this, OpenRecord* openRecord, BYTE* block)
{
	return 0;
}

void TR_allocateNewDirectoryBlock(t2fs_record* this, BYTE* block, DWORD* blockAddress)
{
}
