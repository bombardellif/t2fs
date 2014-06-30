#include "DirectoryBlock.h"
#include "FileSystem.h"

#include <malloc.h>
#include <string.h>

extern FileSystem fileSystem;

#define numOfEntriesInBlock(blockSize)      (blockSize / sizeof(Record))

void DB_DirectoryBlock(DirectoryBlock* this, BYTE* block)
{
    this->entries = (Record*) block;
}


Record* DB_findByName(const DirectoryBlock* const this, const char* name)
{
    if (this == NULL || name == NULL)
        return NULL;
	unsigned int count = numOfEntriesInBlock(fileSystem.superBlock.BlockSize);
    for (int i=0; i < count; i++) {
        if ((this->entries[i].TypeVal == TYPEVAL_DIRETORIO || this->entries[i].TypeVal == TYPEVAL_REGULAR)
        && (strcmp(this->entries[i].name, name) == 0)) {
            return & this->entries[i];
        }
    }
    
    return NULL;
}

Record* DB_findEmpty(const DirectoryBlock* const this, const char* const notUsed)
{
	unsigned int count = numOfEntriesInBlock(fileSystem.superBlock.BlockSize);
    for (int i=0; i < count; i++) {
        if (this->entries[i].TypeVal != TYPEVAL_DIRETORIO
        && this->entries[i].TypeVal != TYPEVAL_REGULAR) {
            return & this->entries[i];
        }
    }
    
    return NULL;
}

void DB_forEachEntry(const DirectoryBlock* const this, void(*callback)(const Record* const))
{
    unsigned int count = numOfEntriesInBlock(fileSystem.superBlock.BlockSize);
    for (int i=0; i < count; i++) {
        if (this->entries[i].TypeVal == TYPEVAL_DIRETORIO
        || this->entries[i].TypeVal == TYPEVAL_REGULAR) {
            
            callback(&this->entries[i]);
        }
    }
}