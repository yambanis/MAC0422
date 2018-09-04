/*
 EP1 – MAC422 - Sistemas Operacionais
 
 ALUNOS:
     Bruno Carneiro da Cunha NUSP 10376388
     Guilherme Yambabis      NUSP 8041265
 
 ENUNCIADO:
 a. protegepracaramba <caminho do arquivo>
     Faz com que o arquivo dado como parâmetro tenha proteção 000.
 b. liberageral <caminho do arquivo>
     Faz com que o arquivo dado como parâmetro tenha proteção 777.
 c. rodeveja <caminho do programa>
     Executa o programa indicado utilizando as chamadas fork(), execve() e wait().
     Sua shell dever emitir uma mensagem indicando o código de retorno. Exemplo:
         rodeveja /home/eu/naofaznada
         => programa ‘/home/eu/naofaznada’ retorno
 d. rode <caminho do programa>
     Executa o programa indicado em background utilizando as chamadas fork() e
     execve() . Sua shell deve monopolizar o teclado, a tela deve continuar
     mostrando a saída da sua shell e do programa. Ao contrário do comando anterior,
     a shell não deve retornar o código de saída do programa.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>

#include "ext/buffer.h"
#include "ext/queue.h"

#define PRINTINSTR printf("protegepracaramba <arquivo>\nliberageral <arquivo>\nrodeveja <programa>\nrode <programa>\n");

/**
 * clearNewline - remove o \n do final do string passado
 */
void
clearNewline(char *str) {
    char *newline = strchr(str, 10);
    *newline = '\0';
}

/**
 * fileIsValid - checa se o arquivo passado existe
 */
bool
fileIsValid(char *filename) {
    clearNewline(filename);
    
    struct stat buffer;
    if (stat(filename, &buffer) == 0) return true;
    else {
        fprintf(stderr, "file does not exist!\n");
        return false;
    }
}

/**
 * binIsValid - checa se o programa é executável
 */
bool
binIsValid(char *bin) {
    char *path = getenv("PATH"),
         *token, *ptr,
         *buffer = malloc(strlen(path) + strlen(bin));
    
    if (!buffer) {
        fprintf(stderr, "binIsValid(): not enough space for buffer!");
        return false;
    }
    printf("path %s\n", path);
    bool itRuns = false;
    while ((token = strsep(&path, ":"))) {
        strcat(buffer, token);
        strcat(buffer, "/");
        strcat(buffer, bin);
        if (access(buffer, X_OK) == 0) itRuns = true;
        printf("buffer = %s\n", buffer);
        *buffer = '\0';
    }
    free(buffer);
    return itRuns;
}

/**
 * runBinary - roda o programa em modo paralelo ou não
 */
void
runBinary(char *bin, bool paralelo) {
    assert(bin);
    clearNewline(bin);
    if (binIsValid(bin)) printf("existe\n");
    else printf("n existe\n");
}

/**
 * processArgs - desempilha o primeiro arg e encaminha pra sua função
 */
void
processArgs(Queue *q) {
    assert(q);
    assert(!queue_isEmpty(q));
    
    char *token = queue_dequeue(q),
         *arg   = queue_dequeue(q);
    if (!token || !arg) {
        PRINTINSTR
        return;
    }
    
    if (strncmp(token, "protegepracaramba", 17) == 0) {
        if (fileIsValid(arg)) {
            if (chmod(arg, 000) < 0) fprintf(stderr, "chmod(): erro!");
        }
    }
    else if (strncmp(token, "liberageral", 11) == 0) {
        if (fileIsValid(arg)) {
            if (chmod(arg, 777) < 0) fprintf(stderr, "chmod(): erro!");
        }
    }
    else if (strncmp(token, "rodeveja", 8) == 0) runBinary(arg, false);
    else if (strncmp(token, "rode", 4) == 0) runBinary(arg, true);
    else PRINTINSTR
    
}

/**
 * parseArgs - quebra a linha em palavras e adiciona cada uma à fila
 */
Queue *
parseArgs(Buffer *b) {
    assert(b);
    
    Queue *q = queue_new();
    char *token;
    
    while ((token = strsep((char**) &b -> data, " ")))
        queue_enqueue(q, token, strlen(token));
    
    return(q);
}

/**
 * readLoop - faz o parse do stdin continuamente
 */
void readLoop() {
    Buffer *b = buffer_create(sizeof(char));
    Queue *q;
    int n;
    
    do {
        printf("> ");
        n = read_line(stdin, b);
        q = parseArgs(b);
        processArgs(q);
    } while (n > 0);

    /* limpeza */
    buffer_destroy(b);
    if (q) queue_destroy(q);
}

int main(int argc, char **argv) {
    readLoop();
    return 0;
}  
