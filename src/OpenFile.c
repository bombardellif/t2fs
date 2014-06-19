#include "OpenFile.h"


void OF_OpenFile(OpenFile* this, int recordIndex)
{
    this->recordIndex = recordIndex;
    this->currentPosition = 0;
}
