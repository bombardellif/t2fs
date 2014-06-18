#ifndef T2FS_RECORD_H
#define T2FS_RECORD_H

#include "t2fs.h"
#include "FilePath.h"
#include "OpenRecord.h"

#define T2FS_RECORD_MODIFIED -1

void TR_t2fs_record(t2fs_record* this, byte typeVal, char* name, dword blocksFileSize, dword bytesFileSize);
t2fs_record* TR_find(t2fs_record* this, FilePath filepath, OpenRecord* openRecord, byte* block, byte blockTrace[], dword* recordPointerTrace[], dword blockAddress[]);
int TR_addRecord(t2fs_record* this, t2fs_record newRecord, OpenRecord* newOpenRecord);
void TR_freeBlocks(t2fs_record* this);
t2fs_record* TR_findEmptyEntry(t2fs_record* this, OpenRecord* openRecord, byte* block);
void TR_allocateNewDirectoryBlock(t2fs_record* this, byte* block, dword* blockAddress);


#endif
