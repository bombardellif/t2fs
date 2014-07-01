#ifndef T2FS_RECORD_H
#define T2FS_RECORD_H

#include "t2fs.h"
#include "FilePath.h"
#include "OpenRecord.h"
#include "DirectoryBlock.h"
#include "FileSystem.h"

void TR_Record(Record* this, BYTE typeVal, char* name, DWORD blocksFileSize, DWORD bytesFileSize);
/** @TODO */
void TR_destroy(Record* this);
Record* TR_find(Record* this, FilePath* const filePath, OpenRecord* openRecord, BYTE* block, Record*(*find)(const DirectoryBlock* const this, const char* const notUsed), BYTE blockTrace[][FS_BLOCK_TRACE_DEPTH_LENGTH], DWORD* blockAddressTrace[]);
Record* TR_findRecordInRecord(Record* this, OpenRecord* openRecord, BYTE* block, Record*(*find)(const DirectoryBlock* const this, const char* const notUsed), char* param, BYTE blockTrace[][FS_BLOCK_TRACE_DEPTH_LENGTH], DWORD* blockAddressTrace[]);
int TR_addRecord(Record* this, Record newRecord, OpenRecord* newOpenRecord);
int TR_freeBlocks(Record* this);
int TR_allocateNewDirectoryBlock(Record* this, BYTE* block, DWORD* blockAddress);
int TR_allocateNewIndirectionBlock(BYTE* indirectionBlockMem, DWORD* indirectionBlockAddress);
int TR_allocateNewBlock(DWORD* blockAddress);
int TR_findEmptyPositionInArray(const DWORD const dataPtr[], const unsigned int count);
int TR_findBlockByNumber(Record* this, DWORD number, BYTE* block, DWORD* blockAddress, DWORD** blockAddressPtr);
int TR_appendNewBlock(Record* this, DWORD* const blockAddress);

#endif
