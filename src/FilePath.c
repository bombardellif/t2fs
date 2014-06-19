#include "FilePath.h"

#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "t2fs.h"

static const char DS_STR[] = "/";

void FP_FilePath(FilePath* this, char* path)
{
    this->pathWithoutLastNode = this->path = this->currentToken = NULL;
    this->parsed = FALSE;
    this->tokenWasRetrieved = TRUE;
    
    if (path) {

        int pathLen = strlen(path);
        this->path = (char*) calloc(pathLen + 1, sizeof(char));
        strcpy(this->path, path);


        // if the last char of the path is a directory separator, then, erase it
        if (this->path[pathLen-1] == DS) {
            this->path[pathLen-1] = '\0';
        }

        // Set the path without last node:
        char* lastDelim = strrchr(this->path, DS);
        if (lastDelim) {
            int pathUntilDelimSize = lastDelim - this->path + 1;
            this->pathWithoutLastNode = (char*) calloc(pathUntilDelimSize, sizeof(char));
            this->pathLastNode = (char*) calloc(pathLen - pathUntilDelimSize, sizeof(char));

            // copy from path to pathWithoutLastNode until the last delimiter
            *lastDelim = '\0';
            strcpy(this->pathWithoutLastNode, this->path);
            *lastDelim = DS;
            
            // copy from path to pathLastNode since the last delimiter until the end
            strcpy(this->pathLastNode, lastDelim);
        } else {
            this->pathWithoutLastNode = NULL;
            this->pathLastNode = NULL;
        }
        
    }
}

void FP_destroy(FilePath* this)
{
    if (this) {
        if (this->path) {
            free(this->path);
            this->path = NULL;
        }
        if (this->pathWithoutLastNode) {
            free(this->pathWithoutLastNode);
            this->pathWithoutLastNode = NULL;
        }
        if (this->pathLastNode) {
            free(this->pathLastNode);
            this->pathLastNode = NULL;
        }
    }
}

void FP_withoutLastNode(FilePath* this, FilePath* new)
{
    // Create new File Path without the last node of this File Path
	FP_FilePath(new, this->pathWithoutLastNode);
}

char* FP_getLastNode(FilePath* this)
{
    return this->pathLastNode;
}

static char* FP__nextNode(FilePath* this)
{
    if (!this->path) {
        return NULL;
    } else if (this->parsed) {
        return this->currentToken = strtok(NULL, DS_STR);
    } else {
        this->parsed = TRUE;
        return this->currentToken = strtok(this->path, DS_STR);
    }
}

char* FP_getNextNode(FilePath* this)
{
    if (this->tokenWasRetrieved) {
        return FP__nextNode(this);
    } else {
        this->tokenWasRetrieved = TRUE;
        return this->currentToken;
    }
}

BOOL FP_hasNextNode(FilePath* this)
{
    // if the currentToken was already retrieved, then tokenize again for the
    // next token in the path
    if (this->tokenWasRetrieved) {
        
        // this call will set this->currentToken
        FP__nextNode(this);
        this->tokenWasRetrieved = FALSE;
    }
    
    // if this->currentToken is not NULL, then there is a next node in the path
    if (this->currentToken)
        return TRUE;
    else
        return FALSE;
}
