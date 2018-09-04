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

extern char **environ;

/**
 * clearNewLine - remove o \n do final do string passado
 */
void
clearNewLine(char *str) {
    char *newline = strchr(str, 10);
    *newline = '\0';
}

/**
 * fileIsValid - checa se o arquivo passado existe
 */
bool
fileIsValid(char *filename) {
    clearNewLine(filename);
    
    struct stat buffer;
    if (stat(filename, &buffer) == 0) return true;
    else {
        fprintf(stderr, "Arquivo nao existe!\n");
        return false;
    }
}

/**
 * getPath - procura o programa no path e retorna o binário correto
 */
char *
getPath(char *bin) {
    char *path = strdup(getenv("PATH")),
         *token, *ptr,
         *buffer = malloc(strlen(path) + strlen(bin));
    
    if (!buffer) {
        fprintf(stderr, "getPath(): sem espaco suficiente no buffer!");
        return false;
    }

    while ((token = strsep(&path, ":"))) {
        strcat(buffer, token);
        strcat(buffer, "/");
        strcat(buffer, bin);
        if (access(buffer, X_OK) == 0) return buffer;
        /* reseta o buffer */
        *buffer = '\0';
    }
    
    /* tenta rodar no wd */
    if (bin[0] == '.' && bin[1] == '/')
        if (access(bin, X_OK) == 0) return bin;
    
    return NULL;
}

/**
 * runBinary - roda o programa em modo paralelo ou não
 */
void
runBinary(char *bin, bool paralelo) {
    assert(bin);
    clearNewLine(bin);
    if ((bin = getPath(bin))) {
        pid_t pid = fork();
        
        /* PROCESSO FILHO */
        if (pid == 0) {
            if (paralelo) fclose(stdin);
            execve(bin, NULL, environ);
        }
        /* PROCESSO PAI */
        else {
            if (!paralelo) {
                int childStatus;
                pid_t tid = waitpid(pid, &childStatus, 0);
                if (tid > 0) printf("=> programa '%s' %d\n", bin, childStatus);
                else printf("waitpid(): erro\n");
            }
        }
    }
    else printf("executavel nao existe no PATH!\n");
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
    
    if (strcmp(token, "protegepracaramba") == 0) {
        if (fileIsValid(arg)) {
            if (chmod(arg, 000) < 0) fprintf(stderr, "chmod(): erro!");
            else {
                struct stat buf;  
                stat(arg, &buf);
                int statchmod = buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
                printf("novo modo do arquivo: %o\n",statchmod);
            }    
        }
    }
    else if (strcmp(token, "liberageral") == 0) {
        if (fileIsValid(arg)) {
            if (chmod(arg, 0777) < 0) fprintf(stderr, "chmod(): erro!");
            else {
                struct stat buf;  
                stat(arg, &buf);
                int statchmod = buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
                printf("novo modo do arquivo: %o\n",statchmod);
            }    
        }
    }
    else if (strcmp(token, "rodeveja") == 0) runBinary(arg, false);
    else if (strcmp(token, "rode") == 0) runBinary(arg, true);
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
