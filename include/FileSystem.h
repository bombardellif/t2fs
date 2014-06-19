#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "t2fs.h"
#include "OpenFile.h"
#include "OpenRecord.h"
#include "FilePath.h"
#include "DirectoryBlock.h"

#define FS_SUPERBLOCK_ADDRESS 0
#define FS_NULL_BLOCK_POINTER 0xFFFFFFFF

#define FS_CREATESUCCESS_BUT_OPENPROBLEM 99
#define FS_CREATEPROBLEM_INVALID_PATH 99
#define FS_CREATE_FAIL 99
#define FS_OPENPROBLEM_FILEPOINTER_SPACE_UNAVAILABLE 99
#define FS_OPENPROBLEM_RECORDPOINTER_SPACE_UNAVAILABLE 99

#define FS_OPENFILES_MAXSIZE 20
#define FS_OPENRECORDS_MAXSIZE 20

typedef struct s_FileSystem{
	SuperBlock superBlock;
	OpenFile openFiles[FS_OPENFILES_MAXSIZE];
	OpenRecord openRecords[FS_OPENRECORDS_MAXSIZE];
	DWORD SUPERBLOCK_ADDRESS;
} FileSystem;

t2fs_file FS_create(FilePath* const filePath);
t2fs_file FS_createHandle(OpenRecord openRecord);
Record* FS_findRecordInArray(DWORD dataPtr[], BYTE* block, DWORD* blockAddress, Record*(*find)(DirectoryBlock*, char* param), char* name, int count);
int FS_findEmptyInArray(DWORD dataPtr[], BYTE* block, DWORD* blockAddress, int count);
int FS_delete(FilePath* const filePath);
t2fs_file FS_open(FilePath* const filePath);
#endif
