/*
 EP1 – MAC422 - Sistemas Operacionais
 
 ALUNOS:
     Bruno Carneiro da Cunha NUSP 10376388
     Guilherme Yambabis      NUSP
 
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

#include "ext/buffer.h"
#include "ext/queue.h"

/**
 * parseArgs - quebra o input e processa
 */
char **
parseArgs(Buffer *buffer) {
    char *c;
    printf("buffer = %s", (char*) buffer -> data);
    return(buffer -> data);
}

/**
 * readLoop - faz o parse do stdin continuamente
 */
void readLoop() {
    Buffer *buffer = buffer_create(sizeof(char));
    char **args;

    do {
        printf("> ");
        read_line(stdin, buffer);
        args = parseArgs(buffer);
    } while (true);


    free(buffer);
}

int main(int argc, char **argv) {
    readLoop();
    return 0;
}
