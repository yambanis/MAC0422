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
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>

#define PRINTINSTR printf("protegepracaramba <arquivo>\nliberageral <arquivo>\nrodeveja <programa>\nrode <programa>\n");

extern char **environ;

/**
 * clearNewLine - remove o \n do final do string passado
 */
void
clearNewLine(char *str) {
    str[strlen(str) -1] = '\0';
}

/**
 * fileIsValid - checa se o arquivo passado existe
 */
char
fileIsValid(char *filename) {
    clearNewLine(filename);
    struct stat buffer;
    if (stat(filename, &buffer) == 0) return 1;
    else {
        fprintf(stderr, "Arquivo nao existe!\n");
        return 0;
    }
}

/**
 * getPath - procura o programa no path e retorna o binário correto
 */
char *
getPath(char *bin) {
    int pathSize = strlen(getenv("PATH"));
    char *path = malloc(sizeof(char) * (pathSize + 1)),
         *token, *ptr,
         *buffer = malloc(strlen(path) * (sizeof(char)) + strlen(bin));
    
    if (!buffer) {
        fprintf(stderr, "getPath(): sem espaco suficiente no buffer!");
        return NULL;
    }

    clearNewLine(bin);
    /* copia o PATH e coloca a terminacao no final da str */
    strcpy(path, getenv("PATH"));
    path[pathSize] = '\0';

    token = strtok(path, ":");
    while (token != NULL) {
        strcat(buffer, token);
        strcat(buffer, "/");
        strcat(buffer, bin);
        if (access(buffer, X_OK) == 0){
            free(path);
            return buffer;
        }    
        /* reseta o buffer */
        *buffer = '\0';
        /*busca o proximo path*/
        token = strtok(NULL, ":");
    }
    
    /* tenta rodar no wd */
    if (bin[0] == '.' && bin[1] == '/')
        if (access(bin, X_OK) == 0){ 
            free(path);
            return bin;
        }    
    
    free(path);
    return NULL;
}

/**
 * runBinary - roda o programa em modo paralelo ou não
 */
void
runBinary(char *bin, char paralelo) {
    assert(bin);

    if ((bin = getPath(bin))) {
        pid_t pid = fork();
        
        /* PROCESSO FILHO */
        if (pid == 0) {
            if (paralelo){
                fclose(stdin);
                fclose(stderr);
                fclose(stdout);
            }    
            execve(bin, NULL, environ);
            free(bin);
        }
        /* PROCESSO PAI */
        else {
            if (!paralelo) {
                int childStatus;
                pid_t tid = waitpid(pid, &childStatus, 0);
                if (tid > 0) printf("=> programa '%s' retornou %d\n", bin, WEXITSTATUS(childStatus));
                else printf("waitpid(): erro\n");
                free(bin);
            }
        }
    }
    else printf("executavel nao existe no PATH!\n");
    free(bin);
}

/**
 * processArgs - desempilha o primeiro arg e encaminha pra sua função
 */
void
processArgs(char **args) {
    struct stat buf;
    int statchmod;
    char *comando = args[0],
         *arg = args[1];

    if (!comando || !arg) {
        PRINTINSTR
        return;
    }
    
    if (strcmp(comando, "protegepracaramba") == 0) {
        if (fileIsValid(arg)) {
            if (chmod(arg, 000) < 0) fprintf(stderr, "chmod(): erro!");
            else {
                stat(arg, &buf);
                statchmod = buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
                printf("novo modo do arquivo: %o\n",statchmod);
            }    
        }
    }
    else if (strcmp(comando, "liberageral") == 0) {
        if (fileIsValid(arg)) {
            if (chmod(arg, 0777) < 0) fprintf(stderr, "chmod(): erro!");
            else {
                stat(arg, &buf);
                statchmod = buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
                printf("novo modo do arquivo: %o\n",statchmod);
            }    
        }
    }
    else if (strcmp(comando, "rodeveja") == 0) runBinary(arg, 0);
    else if (strcmp(comando, "rode") == 0) runBinary(arg, 1);
    else PRINTINSTR
        
        }

/**
 * parseArgs - quebra a linha em palavras e adiciona cada uma à fila
 */
char **
parseArgs(char *line) {
    int i = 0;
    char *token, *ptr;
    char **args = malloc(sizeof(char*) * 2);
    if(!args) {
        fprintf(stderr, "parseArgs(): nao ha espaco suficiente pros args!\n");
        return NULL;
    }

    token = strtok(line, " ");
    while(i < 2){
        args[i] = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(args[i], token);
        args[i][strlen(token)] = '\0';
        i++;
        token = strtok(NULL, " ");
    }

    return(args);
}

/**
 * readLoop - faz o parse do stdin continuamente
 */
void
readLoop(void) {
    char *buffer = malloc(sizeof(char) * 256);
    char **args;
    int i = 0;
    if (!buffer) {
        fprintf(stderr, "readLoop(): nao ha espaco suficiente pro buffer!\n");
        return;
    }   
    printf("> ");
    while (fgets(buffer, 256, stdin) != NULL){
        if (buffer[0] != '\n'){
            args = parseArgs(buffer);
            processArgs(args);
        }
    printf("> ");
    }

    free(buffer);
    if (args) {
        while(i<2) free(args[i++]);
        free(args);
    }
}

int main(int argc, char **argv) {
    readLoop();
    return 0;
}

