#ifndef FILE_PATH_H
#define FILE_PATH_H

#include "t2fs.h"

#define DS '/'

typedef struct s_FilePath {
	char* path;
    char* pathWithoutLastNode;
    char* pathLastNode;
    BOOL parsed;
    char* currentToken;
    BOOL tokenWasRetrieved;
} FilePath;

void FP_FilePath(FilePath* this, char* path);
void FP_destroy(FilePath* this);
void FP_withoutLastNode(FilePath* this, FilePath* new);
char* FP_getLastNode(FilePath* this);
char* FP_getNextNode(FilePath* this);
BOOL FP_hasNextNode(FilePath* this);

#endif
