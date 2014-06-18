#include "t2fs_record.h"


void t2fs_record::t2fs_record(t2fs_record* this, byte typeVal, java::lang::String name, dword blocksFileSize, dword bytesFileSize)
{
}

t2fs_record* t2fs_record::find(t2fs_record* this, FilePath filepath, OpenRecord* openRecord, Block* block, Block blockTrace[], dword* recordPointerTrace[], dword blockAddress[])
{
	return 0;
}

int t2fs_record::addRecord(t2fs_record* this, t2fs_record newRecord, OpenRecord* newOpenRecord)
{
	return 0;
}

void t2fs_record::freeBlocks(t2fs_record* this)
{
}

t2fs_record* t2fs_record::findEmptyEntry(t2fs_record* this, OpenRecord* openRecord, Block* block)
{
	return 0;
}

void t2fs_record::allocateNewDirectoryBlock(t2fs_record* this, Block* block, dword* blockAddress)
{
}
