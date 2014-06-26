#include "t2fs.h"
#include "FilePath.h"
#include "FileSystem.h"

#include <stdlib.h>
#include <string.h>
#include <malloc.h>

static const char IDENTIFY[] = "Fernando Bombardelli da Silva (218324) - William Bombardelli da Silva (218323)";

char *t2fs_identify (void)
{
    char* returnValue = (char*)calloc(strlen(IDENTIFY)+1, sizeof(char));
    
    strcpy(returnValue, IDENTIFY);
    
    return returnValue;
}

t2fs_file t2fs_create (char *nome)
{
    FilePath filePath;
    FP_FilePath(&filePath, nome);
    
    t2fs_file returnValue = FS_create(&filePath);
    FP_destroy(&filePath);
    
    if(returnValue < 0){
        returnValue = T2FS_INVALID_ARGUMENT;
    }
    
    return returnValue;
}

int t2fs_delete (char *nome)
{
    FilePath filePath;
    FP_FilePath(&filePath, nome);
    
    int returnValue = FS_delete(&filePath);
    FP_destroy(&filePath);
    
    return returnValue;
}

t2fs_file t2fs_open (char *nome)
{
    FilePath filePath;
    FP_FilePath(&filePath, nome);
    
    t2fs_file returnValue = FS_open(&filePath);
    FP_destroy(&filePath);
    
    return returnValue;
}

int t2fs_close (t2fs_file handle)
{
    return FS_close(handle);
}

int t2fs_read (t2fs_file handle, char *buffer, int size)
{
    return FS_read(handle, buffer, size);
}

int t2fs_write (t2fs_file handle, char *buffer, int size)
{
    return FS_write(handle, buffer, size);
}

int t2fs_seek (t2fs_file handle, unsigned int offset)
{
    return FS_seek(handle, offset);
}