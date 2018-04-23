//Compilar utilizando:  gcc teste1myMyMalloc.c -o testeAA3 -lm parametroTAM 
//Caso necessário adicionar algo para calcular o tempo

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "libmem.h"

int **Matriz1;
int **Matriz2;
int **Matriz3;

int nLinhas1,nColunas1,nLinhas2,nColunas2;

typedef struct pos {
   int linha;
   int coluna;
}POS;

int** ler_matrix(int linha, int coluna)
{
    int** matriz;
    int i,j;

    matriz = (int **)MyMalloc(linha*sizeof(int*));
    for(i = 0;i<linha;i++) {
        matriz[i] = (int *)MyMalloc(coluna*sizeof(int));
    }
    for(i = 0;i<linha;i++) {
        for(j = 0;j<coluna;j++) {
            scanf("%d", &matriz[i][j]);
        }
    }
    return matriz;
}

void imprimir_matriz(int **Matriz, int linha, int coluna){
    int i, j;
    for(i=0;i<linha;i++){
        for(j=0;j<coluna;j++){
            printf("%d \t",Matriz[i][j]);
        }
    printf("\n");
    }
    printf("\n");
}

void *thread_Multiplica(void *);

int main(int argc, char* argv[])
{
    int i=0;
    pthread_t *tid;
    int linha,coluna;
    POS *data;
    int tam;

    Matriz1 = ler_matrix(nLinhas1,nColunas1);
    Matriz2 = ler_matrix(nLinhas2,nColunas2);

    tam = atoi(argv[1]);//QUANTIDADE DE LINHAS E COLUNAS DE CADA MATRIZ
    nLinhas1 = tam; nColunas1 = tam; nLinhas2 = tam; nColunas2 = tam;
    //POPULA MATRIZ 1 E 2
    for(linha=0;linha<tam;linha++){
		for(coluna=0;coluna<tam;coluna++){
			Matriz1[linnha][coluna] = rand()%tam;
			Matriz2[linnha][coluna] = rand()%tam;
		}
	}
/* 
    printf("\nMatriz 1:\n");
    imprimir_matriz(Matriz1, nLinhas1, nColunas1);


    printf("\nMatriz 2:\n");
    imprimir_matriz(Matriz2, nLinhas2, nColunas2);
*/
    Matriz3 = (int**)MyMalloc(nLinhas1*sizeof(int*));
    for(i = 0;i<nLinhas1;i++) {
        Matriz3[i] = (int*)MyMalloc(nColunas2*sizeof(int));
    }
    /////////////////////////////////////////////////////////

    //INICIALIZA VARIAVEIS
    data = MyMalloc(nLinhas1*nColunas2*sizeof(POS));//VETOR DE STRUCTS CONTENDO CADA POSIÇAO DA MATRIZ RESULTADO
    tid = (pthread_t *)MyMalloc(nLinhas1*nColunas2*sizeof(pthread_t));//CRIA VETOR DE ID THREADS
    ///////////////////////

    /////////////////////////CRIA AS THREADS
    if(nColunas1!=nLinhas2)
    {
        printf("A multiplicacao nao e possivel.");
    }
    else
    {
        for(linha=0;linha<nLinhas1;linha++)
        {
            for(coluna=0;coluna<nColunas2;coluna++)
            {
                data[linha*nColunas2 + coluna].linha = linha;
                data[linha*nColunas2 + coluna].coluna = coluna;

                //Cria a Thread
                pthread_create(&tid[linha*nColunas2 + coluna], NULL, thread_Multiplica, &data[linha*nColunas2 + coluna]);
            }
        }
    }
    //////////////////////////

    /////ESPERA TODAS THREADS TERMINAREM ANTES DE IMPRIMIR O RESULTADO
    for(i=0;i<nLinhas1*nColunas2;i++){
        pthread_join(tid[i], NULL);
    }
    //////////////////////////
/*
    //IMPRIME RESULTADO
    printf("\nResultado:\n");
    imprimir_matriz(Matriz3, nLinhas1, nColunas2);
*/

    MyMallocFree(Matriz1);
    MyMallocFree(Matriz2);
    MyMallocFree(Matriz3);
    MyMallocFree(data);
    MyMallocFree(tld);

    return 0;
}

void *thread_Multiplica(void *param)
{
    POS *data = param;
    int i, sum = 0;

    //Multiplica linha pela coluna
    for(i = 0; i < nColunas1; i++){
        sum += Matriz1[data->linha][i] * Matriz2[i][data->coluna];
    }


    Matriz3[data->linha][data->coluna] = sum;

}