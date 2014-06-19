#include "OpenRecord.h"


void OR_incCount(OpenRecord* this)
{
}

void OR_equals(OpenRecord* this, OpenRecord other){
    //If they are int the same block, then they are in the same directory. So, if they have the same name they are equal
    if (strcmp(this->record.name, other.record.name) == 0 && this->blockAddress == other.blockAddress){
        return TRUE;
    }else{
        return FALSE;
    }
}
