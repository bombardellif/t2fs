#include "FileSystem.h"


t2fs_file FS_create(FilePath filePath)
{
	return 0;
}

t2fs_file FS_createHandle(OpenRecord record)
{
	return 0;
}

t2fs_record* FS_findRecordInArray(dword dataPtr[], byte* block, dword* blockAddress, char* name, int count)
{
	return 0;
}

int FS_findEmptyInArray(dword dataPtr[], byte* block, dword* blockAddress, int count)
{
	return 0;
}

int FS_delete(FilePath filePath)
{
	return 0;
}

t2fs_file FS_open(FilePath filePath)
{
	return 0;
}
