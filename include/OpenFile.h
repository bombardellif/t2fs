#ifndef OPEN_FILE_H
#define OPEN_FILE_H

typedef struct s_OpenFile{
	int recordIndex;
    unsigned int currentPosition;
} OpenFile;

void OF_OpenFile(OpenFile* openFile, int recordIndex);

#endif
