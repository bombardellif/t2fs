/* 
 * File:   mkdirt2.c
 * Author: fernando
 *
 * Created on 30. Juni 2014, 17:12
 */

#include <stdio.h>
#include <stdlib.h>
#include "t2fs.h"
#include "FilePath.h"
#include "FileSystem.h"

/*
 * 
 */
int main(int argc, char** argv) {

    if (argc < 2) {
        printf("ERRO: Parâmetros inválidos\n");
        return EXIT_FAILURE;
    }
    
    t2fs_file handle = t2fs_open(argv[1]);
    if (handle == 0) {
        printf("ERRO: Diretório já existe\n");
        return EXIT_FAILURE;
    }
    
    // Create the directory;
    FilePath filePath;
    FP_FilePath(&filePath, argv[1]);
    
    t2fs_file dirHandle = FS_create(&filePath, TYPEVAL_DIRETORIO);
    FP_destroy(&filePath);
    
    if (dirHandle < 0) {
        printf("ERRO: Erro ao criar dretório, verifique o caminho indicado");
        return EXIT_FAILURE;
    }
    
    return (EXIT_SUCCESS);
}

