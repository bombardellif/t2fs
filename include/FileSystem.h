#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "t2fs.h"
#include "OpenFile.h"
#include "OpenRecord.h"
#include "FilePath.h"
#include "DirectoryBlock.h"

#define FS_SUPERBLOCK_ADDRESS 0
#define FS_NULL_BLOCK_POINTER 0xFFFFFFFF

#define FS_SUCCESS 0
#define FS_CREATESUCCESS_BUT_OPENPROBLEM -1
#define FS_INVALID_PATH -2
#define FS_CREATE_FAIL -3
#define FS_FILEPOINTER_SPACE_UNAVAILABLE -4
#define FS_RECORDPOINTER_SPACE_UNAVAILABLE -5
#define FS_DELETEPROBLEM_DIRECTORY_NOT_EMPTY -6

#define FS_OPENFILES_MAXSIZE 20
#define FS_OPENRECORDS_MAXSIZE 20
#define FS_OPENFILE_FREE -1
#define FS_TRACE_DEPTH_LENGTH 4
#define FS_BLOCK_TRACE_DEPTH_LENGTH 3

typedef struct s_FileSystem{
	SuperBlock superBlock;
	OpenFile openFiles[FS_OPENFILES_MAXSIZE];
	OpenRecord openRecords[FS_OPENRECORDS_MAXSIZE];
	DWORD SUPERBLOCK_ADDRESS;
} FileSystem;

int FS_initilize();
t2fs_file FS_create(FilePath* const filePath, BYTE typeVal);
t2fs_file FS_createHandle(OpenRecord openRecord);
Record* FS_findRecordInArray(DWORD dataPtr[], BYTE* block, DWORD* blockAddress, Record*(*find)(const DirectoryBlock* const,const char* param), char* name, int count, int* indexFound);
int FS_delete(FilePath* const filePath);
t2fs_file FS_open(FilePath* const filePath);
int FS_close(t2fs_file handle);
int FS_read(t2fs_file handle, char* buffer, int size);
int FS_write(t2fs_file handle, char* buffer, int size);
int FS_seek(t2fs_file handle, unsigned int offset);

//Extra Functions for utilities
int FS_applyCallbackToDirectory(t2fs_file handle, void(*callback)(const Record* const));

#endif
