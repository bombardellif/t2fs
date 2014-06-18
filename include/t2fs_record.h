#ifndef T2FS_RECORD_H
#define T2FS_RECORD_H

#include "t2fs.h"
#include "FilePath.h"
#include "OpenRecord.h"

#define T2FS_RECORD_MODIFIED -1

void TR_t2fs_record(t2fs_record* this, BYTE typeVal, char* name, DWORD blocksFileSize, DWORD bytesFileSize);
t2fs_record* TR_find(t2fs_record* this, FilePath filepath, OpenRecord* openRecord, BYTE* block, BYTE blockTrace[], DWORD* recordPointerTrace[], DWORD blockAddress[]);
int TR_addRecord(t2fs_record* this, t2fs_record newRecord, OpenRecord* newOpenRecord);
void TR_freeBlocks(t2fs_record* this);
t2fs_record* TR_findEmptyEntry(t2fs_record* this, OpenRecord* openRecord, BYTE* block);
void TR_allocateNewDirectoryBlock(t2fs_record* this, BYTE* block, DWORD* blockAddress);


#endif
