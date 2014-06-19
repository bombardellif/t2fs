#ifndef OPEN_RECORD_H
#define OPEN_RECORD_H

#include "t2fs.h"

typedef struct s_OpenRecord{
	Record record;
	DWORD blockAddress;
	unsigned int count;
} OpenRecord;

void OR_incCount(OpenRecord* this);
BOOL OR_equals(const OpenRecord* const this, const OpenRecord other);

#endif
