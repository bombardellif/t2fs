#include "DirectoryBlock.h"
#include "FileSystem.h"

#include <malloc.h>

extern FileSystem fileSystem;

void DB_DirectoryBlock(DirectoryBlock* this, BYTE* block)
{
    this->entries = (Record*) block;
}


Record* DB_findByName(DirectoryBlock* this, char* name)
{
	unsigned int count = fileSystem.superBlock.BlockSize / sizeof(Record);
    for (int i=0; i < count; i++) {
        if (strcmp(this->entries[i].name, name) == 0) {
            return & this->entries[i];
        }
    }
    
    return NULL;
}
