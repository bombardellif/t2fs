#include "DirectoryBlock.h"
#include "FileSystem.h"

#include <malloc.h>
#include <string.h>

extern FileSystem fileSystem;

void DB_DirectoryBlock(DirectoryBlock* this, BYTE* block)
{
    this->entries = (Record*) block;
}


Record* DB_findByName(const DirectoryBlock* const this, const char* name)
{
    if (this == NULL || name == NULL)
        return NULL;
	unsigned int count = fileSystem.superBlock.BlockSize / sizeof(Record);
    for (int i=0; i < count; i++) {
        if (strcmp(this->entries[i].name, name) == 0) {
            return & this->entries[i];
        }
    }
    
    return NULL;
}

Record* DB_findEmpty(const DirectoryBlock* const this, const char* const notUsed)
{
	unsigned int count = fileSystem.superBlock.BlockSize / sizeof(Record);
    for (int i=0; i < count; i++) {
        if (this->entries[i].TypeVal == TYPEVAL_INVALIDO) {
            return & this->entries[i];
        }
    }
    
    return NULL;
}