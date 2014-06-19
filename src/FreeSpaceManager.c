#include "FreeSpaceManager.h"

#include "FileSystem.h"
#include "DiscAccessManager.h"
#include "t2fs_record.h"

#include <assert.h>
#include <string.h>

#define FSM_BITS_PER_BYTE 8
#define FSM_FULL_BITMAP_BYTE 0xFF

extern FileSystem fileSystem;

#define FSM_blockNumberInBitmapFile(add, blockSize) (add / (FSM_BITS_PER_BYTE * blockSize))
#define FSM_byteOffsetInBlock(add, blockSize)       ((add / FSM_BITS_PER_BYTE) % blockSize)
#define FSM_bitOffsetInByte(add)    (address % FSM_BITS_PER_BYTE)

int FSM_delete(DWORD address)
{
    DWORD blockAddress;
    BYTE block[fileSystem.superBlock.BlockSize];
    int returnCode;
    unsigned int blockNumberInBitmapFile = FSM_blockNumberInBitmapFile(address, fileSystem.superBlock.BlockSize);
    unsigned int byteOffsetInBlock = FSM_byteOffsetInBlock(address, fileSystem.superBlock.BlockSize);
    unsigned short bitOffsetInByte = FSM_bitOffsetInByte(address);
    
    // Read the proper block from disc
    if ((returnCode = TR_findBlockByNumber(&fileSystem.superBlock.BitMapReg, blockNumberInBitmapFile, &block, &blockAddress)) == 0) {
        
        // set to zero the correspondent bit of the bitmap for this address
        block[byteOffsetInBlock] &= ~(0x80 >> bitOffsetInByte);

        returnCode = DAM_write(blockAddress, &block);
    }
    
    return returnCode;
}

int FSM_getFreeAddress(DWORD* address)
{
    DWORD blockAddress;
    BYTE block[fileSystem.superBlock.BlockSize + 1]; // allocate one more byte for a null-termination character
    int returnCode;
    unsigned int blockNumberInBitmapFile = 0;
    char fullBitmapStrAux[2] = {FSM_FULL_BITMAP_BYTE, '\0'};
    
    // place a null after the block data, it is necessary to use the string function below
    block[fileSystem.superBlock.BlockSize] = '\0';
    
    // while haven't gotten to the end of the bitmap file, read the next block
    size_t fullBytesInBlock;
    while (!returnCode) {
        
        if ((returnCode = TR_findBlockByNumber(&fileSystem.superBlock.BitMapReg, blockNumberInBitmapFile, &block, &blockAddress)) == 0) {
            
            fullBytesInBlock = strspn(block, fullBitmapStrAux);
            
            // if the number of full bytes is not equals to the size of the block,
            // then there is a free space here!
            if (fullBytesInBlock != fileSystem.superBlock.BlockSize) {
                
                // Address calc Level 1: number of full blocks in the bitmap
                *address = blockNumberInBitmapFile * fileSystem.superBlock.BlockSize;
                
                // Address calc Level 2: number of full bytes in the block
                *address += fullBytesInBlock;
                
                // Address calc Level 3: the first free bit in the bytes
                unsigned short bitMask = 0x7F; // 01111111
                unsigned short bitOffsetInByte;
                for (bitOffsetInByte=0; bitOffsetInByte<FSM_BITS_PER_BYTE; bitOffsetInByte++) {
                    
                    // if the OR operation with the mask results in something different from 0XFF,
                    // then the current iteration indicates the free bit in the byte
                    if ((block[fullBytesInBlock] | bitMask) != 0XFF) {
                        break;
                    }
                    bitMask >>= 1;
                }
                
                assert(bitOffsetInByte < FSM_BITS_PER_BYTE);
                
                *address += bitOffsetInByte;
                
                break; // it must jump to the bottom of this function, returning 0
            }
        }
        
        blockNumberInBitmapFile++;
    }
    
    return returnCode;
}

int FSM_markAsUsed(DWORD address)
{
    DWORD blockAddress;
    BYTE block[fileSystem.superBlock.BlockSize];
    int returnCode;
    unsigned int blockNumberInBitmapFile = FSM_blockNumberInBitmapFile(address, fileSystem.superBlock.BlockSize);
    unsigned int byteOffsetInBlock = FSM_byteOffsetInBlock(address, fileSystem.superBlock.BlockSize);
    unsigned short bitOffsetInByte = FSM_bitOffsetInByte(address);
    
    // Read the proper block from disc
    if ((returnCode = TR_findBlockByNumber(&fileSystem.superBlock.BitMapReg, blockNumberInBitmapFile, &block, &blockAddress)) == 0) {
        
        // set to 1 the correspondent bit of the bitmap for this address
        block[byteOffsetInBlock] |= 0x80 >> bitOffsetInByte;

        returnCode = DAM_write(blockAddress, &block);
    }
    
    return returnCode;
}
