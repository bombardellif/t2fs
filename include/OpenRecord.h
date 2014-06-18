#ifndef OPEN_RECORD_H
#define OPEN_RECORD_H

#include "t2fs.h"

typedef struct s_OpenRecord{
	t2fs_record record;
	dword blockAddress;
	unsigned int count;
} OpenRecord;

void OR_incCount(OpenRecord* openRecord);

#endif
