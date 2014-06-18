#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "t2fs.h"
#include "OpenFile.h"
#include "OpenRecord.h"
#include "FilePath.h"

typedef struct s_FileSystem{
	t2fs_superbloco superBlock;
	OpenFile openFiles[];
	OpenRecord openRecords[];
	DWORD SUPERBLOCK_ADDRESS;
} FileSystem;

t2fs_file FS_create(FilePath* this);
t2fs_file FS_createHandle(FilePath* this, OpenRecord record);
t2fs_record* FS_findRecordInArray(DWORD dataPtr[], BYTE* block, DWORD* blockAddress, char* name, int count);
int FS_findEmptyInArray(DWORD dataPtr[], BYTE* block, DWORD* blockAddress, int count);
int FS_delete(FilePath* this);
t2fs_file FS_open(FilePath* this);
#endif
