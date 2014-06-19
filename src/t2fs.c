#include "t2fs.h"
#include "FilePath.h"
#include "FileSystem.h"
#include <stdlib.h>

t2fs_file t2fs_create (char *nome){
    FilePath filePath;
    FP_FilePath(&filePath, nome);
    
    t2fs_file returnValue = FS_create(&filePath);    
    FP_destroy(&filePath);
    
    if(returnValue < 0){
        returnValue = CREATE_INVALID_FILENAME;
    }
    
    return returnValue;
}