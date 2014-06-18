#ifndef DISC_ACCESS_MANAGER_H
#define DISC_ACCESS_MANAGER_H

#include "t2fs.h"

int DAM_write(dword blockAddress, byte* data, int size);
int DAM_read(dword blockAddress, byte* data, int size);

#endif
