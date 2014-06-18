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
	dword SUPERBLOCK_ADDRESS;
} FileSystem;

t2fs_file FS_create(FilePath filePath);
t2fs_file FS_createHandle(OpenRecord record);
t2fs_record* FS_findRecordInArray(dword dataPtr[], byte* block, dword* blockAddress, char* name, int count);
int FS_findEmptyInArray(dword dataPtr[], byte* block, dword* blockAddress, int count);
int FS_delete(FilePath filePath);
t2fs_file FS_open(FilePath filePath);
#endif
