#include "DiscAccessManager.h"

#include "apidisk.h"
#include "FileSystem.h"

extern FileSystem fileSystem;

int DAM_write(DWORD blockAddress, BYTE* data)
{
    int successCode = 0;
    if (blockAddress != FS_NULL_BLOCK_POINTER) {
        unsigned int sector, numOfSectors;
        if (blockAddress == FS_SUPERBLOCK_ADDRESS) {
            numOfSectors = fileSystem.superBlock.SuperBlockSize;
            sector = FS_SUPERBLOCK_ADDRESS;
        } else {
            numOfSectors = fileSystem.superBlock.BlockSize / BYTES_PER_SECTOR;
            sector = (blockAddress / numOfSectors) + fileSystem.superBlock.SuperBlockSize;
        }

        for (; numOfSectors>0; numOfSectors--) {
            successCode = write_sector(sector, (char*)data);

            if (successCode)
                break;

            data += BYTES_PER_SECTOR;
        }
    } else {
        successCode = DAM_ERROR_NULL_DISC_POINTER;
    }
    
    return successCode;
}

int DAM_read(DWORD blockAddress, BYTE* data)
{
    int successCode = 0;
    if (blockAddress != FS_NULL_BLOCK_POINTER) {
        unsigned int sector, numOfSectors;
        if (blockAddress == FS_SUPERBLOCK_ADDRESS) {
            numOfSectors = fileSystem.superBlock.SuperBlockSize;
            sector = FS_SUPERBLOCK_ADDRESS;
        } else {
            numOfSectors = fileSystem.superBlock.BlockSize / BYTES_PER_SECTOR;
            sector = (blockAddress / numOfSectors) + fileSystem.superBlock.SuperBlockSize;
        }

        int successCode = 0;
        for (; numOfSectors>0; numOfSectors--) {
            successCode = read_sector(sector, (char*)data);

            if (successCode)
                break;

            data += BYTES_PER_SECTOR;
        }
    } else {
        successCode = DAM_ERROR_NULL_DISC_POINTER;
    }
    
    return successCode;
}
