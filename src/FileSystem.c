#include "FileSystem.h"

t2fs_file FS_create(FilePath* this)
{
	return 0;
}

t2fs_file FS_createHandle(FilePath* this, OpenRecord record)
{
	return 0;
}

t2fs_record* FS_findRecordInArray(DWORD dataPtr[], BYTE* block, DWORD* blockAddress, char* name, int count)
{
	return 0;
}

int FS_findEmptyInArray(DWORD dataPtr[], BYTE* block, DWORD* blockAddress, int count)
{
	return 0;
}

int FS_delete(FilePath* this)
{
	return 0;
}

t2fs_file FS_open(FilePath* this)
{
	return 0;
}
