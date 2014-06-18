

#ifndef T2FS_H
#define T2FS_H

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned int DWORD;
typedef int BOOL;
#define FALSE 0
#define TRUE 1

typedef int t2fs_file;

#define TYPEVAL_REGULAR     0x01
#define TYPEVAL_DIRETORIO   0x02
#define TYPEVAL_INVALIDO    0xFF    // qualquer outro valor também é invalido

/** Registro de diretório (entrada de diretório) */
struct t2fs_record {
    /* Tipo da entrada. Indica se o registro é válido e, se for, o tipo do arquivo (regular ou diretório).
    •	0xFF, registro inválido (não associado a nenhum arquivos);
    •	0x01 arquivo regular;
    •	0x02, arquivo de diretório.
    */
    BYTE    TypeVal;        //   0:  1 byte

    /* Nome do arquivo. : string com caracteres ASCII (0x21 até 0x7A), case sensitive.
    O string deve terminar com o caractere especial “\0” (0x00). */
    char    name[39];       //   1: 39 bytes

    /* Tamanho do arquivo. Expresso, apenas, em número de blocos de dados
    (não estão inclusos eventuais blocos de índice). */
    DWORD   blocksFileSize; //  40:  4 bytes

    /* Tamanho do arquivo. Expresso em número de bytes.
    Notar que o tamanho de um arquivo não é um múltiplo do tamanho dos blocos de dados.
    Portanto, o último bloco de dados pode não estar totalmente utilizado.
    Assim, a detecção do término do arquivo dependerá da observação desse campo do registro. */
    DWORD   bytesFileSize;  //  44:  4 bytes

    /* Dois ponteiros diretos, para blocos de dados do arquivo */
    DWORD   dataPtr[2];     //  48:  8 bytes

    /* Ponteiro de indireção simples,
    que aponta para um bloco de índices onde estão ponteiros para blocos de dados do arquivo. */
    DWORD   singleIndPtr;   //  56:  4 bytes

    /* Ponteiro de indireção dupla,
    que aponta para um bloco de índices onde estão outros ponteiros para blocos de índice.
    Esses últimos ponteiros apontam para blocos de dados do arquivo. */
    DWORD   doubleIndPtr;   //  60:  4 bytes

} __attribute__((packed));

/** Superbloco */
struct t2fs_superbloco {
    /* Identificação do sistema de arquivo. É formado pelas letras “T2FS”. */
    char    Id[4];          // :   4 bytes

    /* Versão atual desse sistema de arquivos: (valor fixo 7DE=2014; 1=1 semestre). */
    WORD    Version;        // :   2 bytes

    /* Quantidade de setores  que formam o superbloco. (fixo em 1 setor) */
    WORD    SuperBlockSize; // :   2 bytes

    /* Tamanho total da partição T2FS, incluindo o tamanho do superblock. (1.048.832 bytes) */
    DWORD   DiskSize;       // :   4 bytes

    /* Quantidade total de blocos de dados na partição T2FS (1024 blocos). */
    DWORD   NofBlocks;      // :   4 bytes

    /* Tamanho de um bloco. (1024 bytes) */
    DWORD   BlockSize;      // :   4 bytes

    /* Não usados */
    char    Reserved[108];  // : 108 bytes

    /* Registro que descreve o arquivo que mantém o bitmap de blocos livres e ocupados */
    struct t2fs_record BitMapReg;  // :  64 bytes

    /* Registro que descreve o arquivo que mantém as entradas do diretório raiz */
    struct t2fs_record RootDirReg; // :  64 bytes

} __attribute__((packed));

/** Retorna a identificação dos implementadores do T2FS. */
char *t2fs_identify (void);

/** Função usada para criar um novo arquivo no disco. */
t2fs_file t2fs_create (char *nome);

/** Função usada para remover (apagar) um arquivo do disco. */
int t2fs_delete (char *nome);

/** Função que abre um arquivo existente no disco. */
t2fs_file t2fs_open (char *nome);

/** Função usada para fechar um arquivo. */
int t2fs_close (t2fs_file handle);

/** Função usada para realizar a leitura em um arquivo. */
int t2fs_read (t2fs_file handle, char *buffer, int size);

/** Função usada para realizar a escrita em um arquivo. */
int t2fs_write (t2fs_file handle, char *buffer, int size);

/** Altera o contador de posição (current pointer) do arquivo. */
int t2fs_seek (t2fs_file handle, unsigned int offset);


#endif
