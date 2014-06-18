
#ifndef APIDISK_H
#define APIDISK_H

#define BYTES_PER_SECTOR 0x100   /* Setores do disco possuem 256 bytes */

/**
Função: Realiza a leitura do setor “sector” do disco e coloca os bytes lidos no espaço de memória indicado pelo ponteiro “buffer”.
Retorna “0”, se a leitura foi realizada corretamente e um valor diferente de zero, caso tenha ocorrido algum erro.
*/
int read_sector (unsigned int sector, char *buffer);

/**
Função: Realiza a escrita do conteúdo da memória indicada pelo ponteiro “buffer” no setor “sector” do disco.
Retorna “0”, se a escrita foi bem sucedida; retorna um valor diferente de zero, caso tenha ocorrido algum erro.
*/
int write_sector (unsigned int sector, char *buffer);

#endif
