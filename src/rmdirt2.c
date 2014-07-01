/* 
 * File:   rmdirt2.c
 * Author: fernando
 *
 * Created on 30. Juni 2014, 18:25
 */

#include <stdio.h>
#include <stdlib.h>
#include "t2fs.h"
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
    if (handle < 0) {
        printf("ERRO: Verifique o caminho indicado\n");
        return EXIT_FAILURE;
    }
    
    if (!FS_isDirectory(handle)) {
        printf("ERRO: O caminho não é um diretório\n");
        return EXIT_FAILURE;
    }
    
    if (t2fs_delete(argv[1])) {
        printf("ERRO: Erro ao deletar diretório, ele deve ser vazio\n");
        return EXIT_FAILURE;
    }
    
    t2fs_close(handle);
    
    return (EXIT_SUCCESS);
}

