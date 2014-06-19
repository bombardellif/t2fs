#ifndef OPEN_RECORD_H
#define OPEN_RECORD_H

#include "t2fs.h"

typedef struct s_OpenRecord{
	Record record;
	DWORD blockAddress;
	unsigned int count;
} OpenRecord;

void OR_incCount(OpenRecord* this);
void OR_equals(OpenRecord* this, OpenRecord other);

#endif
