#ifndef FILE_PATH_H
#define FILE_PATH_H

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <assert.h>

#include "java/lang/String.h"

class FilePath
{
private:
	java::lang::String path;


public:
	void FilePath(java::lang::String path);

	FilePath withoutLastNode();

	/**
	 *  
	 */
	java::lang::String getNextNode();

	boolean hasNextNode();

};
#endif
