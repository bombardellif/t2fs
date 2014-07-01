/* 
 * File:   copy2t2.c
 * Author: fernando
 *
 * Created on 30. Juni 2014, 22:03
 */

#include <stdio.h>
#include <stdlib.h>

#include "t2fs.h"

/*
 * 
 */
int main(int argc, char** argv) {

    if (argc < 3) {
        printf("ERRO: Parâmetros inválidos\n");
        return EXIT_FAILURE;
    }
    // open file in the native filesystem
    FILE* nativeFile = fopen(argv[1], "r");
    if (nativeFile == NULL) {
        printf("ERRO: Não foi possível abrir %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    
    // check if destiny doesn't exist
    t2fs_file destinyFile = t2fs_open(argv[2]);
    if (destinyFile == 0) {
        printf("ERRO: Arquivo %s já existe\n", argv[2]);
        return EXIT_FAILURE;
    }
    
    // create destiny file
    destinyFile = t2fs_create(argv[2]);
    if (destinyFile != 0) {
        printf("ERRO: Erro ao criar arquivo %s, verifique o caminho\n", argv[2]);
        return EXIT_FAILURE;
    }
    
    // Uses a buffer of 1KB to transfer from one file to the other
    int returnCode = 0;
    int elemsRead;
    char buffer[310 * 1024];
    while (!feof(nativeFile)) {
        
        if ((elemsRead = fread(buffer, 1, 310 * 1024, nativeFile)) != 0) {
            
            if (t2fs_write(destinyFile, buffer, elemsRead) != elemsRead) {
                // error, stop copying
                returnCode = 1;
                break;
            }
        } else {
            if (ferror(nativeFile)) {
                // error, stop copying
                returnCode = 1;
                break;
            }
        }
    }
    
    t2fs_close(destinyFile);
    fclose(nativeFile);
    
    if (returnCode) {
        printf("ERRO: Erro ao copiar arquivo\n");
        return EXIT_FAILURE;
    }
    
    return (EXIT_SUCCESS);
}

