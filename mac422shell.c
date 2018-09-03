/*
 EP1 – MAC422 - Sistemas Operacionais
 
 ALUNOS:
     Bruno Carneiro da Cunha NUSP 10376388
     Guilherme Yambabis      NUSP 8041265
 
 ENUNCIADO:
 a.protegepracaramba <caminho do arquivo>
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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>


#include "ext/buffer.h"
#include "ext/queue.h"

/**
 * parseArgs - quebra o input e processa
 */
Queue *
parseArgs(Buffer *b) {
    Queue *q = queue_new();
    char *token;
    // máx numero de args == 32
    char **args = malloc(sizeof(char*) * 32);
    int i = 0;
    while ((token = strsep((char**) &b -> data, " "))) {
        queue_enqueue(q, token, strlen(token));
        /* free? */
        i++;
    }
    return(q);
}

/**
 * readLoop - faz o parse do stdin continuamente
 */
void readLoop() {
    Buffer *b = buffer_create(sizeof(char));
    Queue *q;
    char *arg;
    int n;
    
    do {
        printf("> ");
        n = read_line(stdin, b);
        q = parseArgs(b);
        
        /* teste */
        while (!queue_isEmpty(q)) {
            arg = queue_dequeue(q);
            printf("arg = %s\n", arg);
        }
        
    } while (n > 0);


    buffer_destroy(b);
    queue_destroy(q);
}

int main(int argc, char **argv) {
    readLoop();

    if(argc != 2) {
        printf("Invalid number of arguments\n");
        return 1;
    }
    char oprt[] = argv[0];

    if(strcmp(oprt, "protegepracaramba") == 0){
        return chmod(argv[1], 000)
    } 
    else if(strcmp(oprt, "liberageral") == 0){
        return chmod(argv[1], 777)
    }
    //else if(strcmp(oprt, "rodeveja") == 0){
    //}
    //else if(strcmp(oprt, "rode") == 0){
    //}
    else{
        printf("Invalid operation\n");
        return 1;
    }


    return 0;
}   

    return 0;
}
