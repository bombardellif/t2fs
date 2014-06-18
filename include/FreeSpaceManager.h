#ifndef FREE_SPACE_MANAGER_H
#define FREE_SPACE_MANAGER_H

#include "t2fs.h"

typedef struct s_FreeSpaceManager{
	t2fs_record bitmap;
} FreeSpaceManager;

void FSM_delete(DWORD address);
DWORD FSM_getFreeAddress();
void FSM_markAsUsed(DWORD address);

#endif
