# protegepracaramba  <caminho do programa>
##	CHMOD(<>, 000)
### Vamos usar a chamada de funcao chmod, contidda em /sys/stat.h com argumento 000, que faz o system call correspondente

# libera geral  <caminho do programa>
## CHMOD(<>, 777)
### Vamos usar a chamada de funcao chmod, contidda em /sys/stat.h com argumento 777, que faz o system call correspondente

# rodeveja <caminho do programa>
## fork(), execve(), wait()

# rode <caminho do programa>
##Executa o programa indicado em background utilizando as chamadas 
​fork() e execve().
Sua shell deve monopolizar o teclado, a tela deve continuar mostrando a
saída da sua shell e do programa. Ao contrário do comando anterior, a shell não deve
retornar o código de saída do programa.