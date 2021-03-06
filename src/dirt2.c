/* 
 * File:   dirt2.c
 * Author: fernando
 *
 * Created on 30. Juni 2014, 15:40
 */

#include <stdio.h>
#include <stdlib.h>
#include "t2fs.h"
#include "FileSystem.h"

void printDirEntry(const Record* const entry) {
    if (entry->TypeVal == TYPEVAL_REGULAR || entry->TypeVal == TYPEVAL_DIRETORIO) {
        printf("%-39s %c %6u %10u bytes\n",
            entry->name,
            (entry->TypeVal == TYPEVAL_REGULAR) ? 'r' : 'd',
            entry->blocksFileSize,
            entry->bytesFileSize
        );
    }
}

/*
 * 
 */
int main(int argc, char** argv) {

    if (argc < 2) {
        printf("ERRO: Parâmetros inválidos\n");
        return EXIT_FAILURE;
    }
    
    t2fs_file dirHandle = t2fs_open(argv[1]);
    
    if (dirHandle < 0) {
        printf("ERRO: Erro ao listar diretório, verifique o caminho indicado\n");
        return EXIT_FAILURE;
    }
    
    if (FS_applyCallbackToDirectory(dirHandle, printDirEntry)) {
        printf("ERRO: Erro ao listar diretório, verifique o caminho indicado\n");
        return EXIT_FAILURE;
    }
    
    t2fs_close(dirHandle);
    
    return (EXIT_SUCCESS);
}

