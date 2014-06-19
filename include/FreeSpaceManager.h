#ifndef FREE_SPACE_MANAGER_H
#define FREE_SPACE_MANAGER_H

#include "t2fs.h"

int FSM_delete(DWORD address);
int FSM_getFreeAddress(DWORD* address);
int FSM_markAsUsed(DWORD address);

#endif
