#include "t2fs.h"
#include <stdlib.h>

t2fs_file t2fs_create (char *nome){
    FilePath filePath;
    FP_FilePath(filePath, nome);
    
    t2fs_file returnValue;
    
    if (filePath != NULL){
        returnValue = FS_create(filePath);
    }else{
        returnValue = CREATE_INVALID_FILENAME;
    }
    
    FP_destroy(filePath);
    return returnValue;
}