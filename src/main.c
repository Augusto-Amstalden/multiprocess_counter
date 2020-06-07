#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>

//=========================================================================================//

// Funcao para verificar se um numero eh primo
  int eh_primo(unsigned int p) {

    int cont = 0;
    unsigned int primos[80] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
    73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
    179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
    283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409};
    if(p < 410) {
      for(int i = 0; i < 80; i++) {
        if(p == primos[i]) {
          cont = 1;
          break;
        }
      }
      return cont;
    }
    else {
      for(unsigned int i = 2; i <= p/2; i++) {
        if(p%i == 0) {
          cont++;
          break;
        }
      }
      if(cont == 1) {
        return 0;
      }
      else {
        return 1;
      }
    }
  }

//=========================================================================================//

  int main() {

    pid_t pid[4] = {-1, -1, -1, -1};
    int mark;

    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANON;

    int *numeros_primos, quantidade = 0; // Contador de numeros primos encotrados
    numeros_primos = (int*) mmap(NULL, sizeof(int), protection, visibility, 0, 0);

//=========================================================================================//

// Colocando a entrada em um vetor de numeros a serem analisados
    unsigned int numeros[512];
    char nums[1024]; // Espaço para os numeros todos como um texto
    scanf(" %[^\n]", nums); // le todos os números como um texto
    char *token = strtok(nums," "); // le ate ao primeiro espaço para a variavel token
    int pos = 0; // Posição para guardar os numeros no vetor começa em 0

    while (token != NULL) {
      numeros[pos++] = atoi(token); // Guarda o valor convertido em numero no vetor e avança
      quantidade++;
      token = strtok(NULL, " "); // le ate ao próximo espaço a partir de onde terminou
    }

//=========================================================================================//

// Inicializando a lista de marcadores de numeros ja analisadios
    int *marcador;
    marcador = (int*) mmap(NULL, sizeof(int)*quantidade, protection, visibility, 0, 0);
    
    
    for(int i = 0; i < quantidade; i++) {
      marcador[i] = 0;
    }
    
    pthread_mutex_t *trava;
    trava = (pthread_mutex_t*) mmap(NULL, sizeof(pthread_mutex_t), protection, visibility, 0, 0);

//=========================================================================================//

// Criando o numero de processos necessarios para varrer a lista de numeros (max de 4 processos)
    if(quantidade > 0)
      pid[0] = fork();
    if(pid[0] != 0 && quantidade > 1)
      pid[1] = fork();
    if(pid[0] != 0 && pid[1] != 0 && quantidade > 2)
      pid[2] = fork();
    if(pid[0] != 0 && pid[1] != 0 && pid[2] != 0 && quantidade > 3)
      pid[3] = fork();

//==========================================================================================//

// Executando os processos para encontrar os numeros primos da entrada
    if(pid[0] == 0 || pid[1] == 0 || pid[2] == 0 || pid[3] == 0) {
      while(1) {
        mark = 0;
        for(int i = 0; i < quantidade; i++) {
          pthread_mutex_lock(trava);
          if(marcador[i] == 0) {
            marcador[i] = 1;
            pthread_mutex_unlock(trava);
            if(eh_primo(numeros[i]))
            (*numeros_primos)++;
          }
          else {
            pthread_mutex_unlock(trava);
            mark++;
          }
        }
        if(mark == quantidade)
          exit(0);
      }
    }

//============================================================================================//

// Executando o processo pai
    else if(pid[0] != 0 && pid[1] != 0 && pid[2] != 0 && pid[3] != 0) {
        waitpid(pid[0], NULL, 0); // Esperando todos os processos filhos terminarem
        waitpid(pid[1], NULL, 0);
        waitpid(pid[2], NULL, 0);
        waitpid(pid[3], NULL, 0);

        printf("%d\n", *numeros_primos); // Quantidade de numeros primos encontrados
    }

//================================================================================================//


  }
