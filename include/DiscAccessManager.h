#ifndef DISC_ACCESS_MANAGER_H
#define DISC_ACCESS_MANAGER_H

#include "t2fs.h"

int DAM_write(DWORD blockAddress, BYTE* data);
int DAM_read(DWORD blockAddress, BYTE* data);

#endif
