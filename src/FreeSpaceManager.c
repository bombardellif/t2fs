#include "FreeSpaceManager.h"

#include "FileSystem.h"

#define BITS_PER_BYTE 8

extern FileSystem fileSystem;

void FSM_delete(DWORD address)
{
    unsigned int blockNumberInBitmapFile = address / (BITS_PER_BYTE * fileSystem.superBlock.BlockSize);
    
}

DWORD FSM_getFreeAddress()
{
	return 0;
}

void FSM_markAsUsed(DWORD address)
{
}
