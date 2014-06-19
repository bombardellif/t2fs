#ifndef T2FS_RECORD_H
#define T2FS_RECORD_H

#include "t2fs.h"
#include "FilePath.h"
#include "OpenRecord.h"

#define T2FS_ADDRECORD_SUCCESS 0
#define T2FS_RECORD_MODIFIED -1
#define T2FS_ADDRECORD_INVALID_ARGUMENT -2
#define T2FS_ADDRECORD_CANT_ALLOCATE -3
#define T2FS_ADDRECORD_IOERROR -4
#define T2FS_ALLOCATENEWDIRBLOCK_SUCCESS 99
#define T2FS_ADDRECORD_DIDNT_FIND -5

void TR_t2fs_record(Record* this, BYTE typeVal, char* name, DWORD blocksFileSize, DWORD bytesFileSize);
Record* TR_find(Record* this, FilePath* const filePath, OpenRecord* openRecord, BYTE* block, Record*(*find)(const DirectoryBlock* const this, const char* const notUsed), BYTE blockTrace[], DWORD* recordPointerTrace[], DWORD blockAddress[]);
Record* TR_findRecordInRecord(Record* this, OpenRecord* openRecord, BYTE* block, Record*(*find)(const DirectoryBlock* const this, const char* const notUsed), char* param);
int TR_allocateNewBlock(DWORD* blockAddress);
int TR_findEmptyPositionInArray(const DWORD const dataPtr[], const unsigned int count);
int TR_addRecord(Record* this, Record newRecord, OpenRecord* newOpenRecord);
int TR_freeBlocks(Record* this);
int TR_allocateNewDirectoryBlock(Record* this, BYTE* block, DWORD* blockAddress);
int TR_findBlockByNumber(Record* this, DWORD number, BYTE* block, DWORD* blockAddress);

#endif
