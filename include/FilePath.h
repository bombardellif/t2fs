#ifndef FILE_PATH_H
#define FILE_PATH_H

typedef struct s_FilePath {
	char* path;
} FilePath;

void FilePath(FilePath* this, char* path);
FilePath withoutLastNode(FilePath* this);
char* getNextNode(FilePath* this);
BOOL hasNextNode(FilePath* this);

#endif
