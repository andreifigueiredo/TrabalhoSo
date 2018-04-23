#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

extern errno;

/* acessa um semaforo ja criado */
int GetSem( key_t key)
{
  int semid;
  semid = semget(key, 1, 0777 | IPC_CREAT);
  return(semid);
}


/* define operacao DOWN */
void DOWN( int semid)
{
  struct sembuf psem[2];

  psem[0].sem_num = 0;
  psem[0].sem_op = -1;
  psem[0].sem_flg = SEM_UNDO;
  
  semop(semid, psem, 1);
}

/* define operacao UP */
void UP( int semid)
{
  struct sembuf vsem[2];

  vsem[0].sem_num = 0;
  vsem[0].sem_op = 1;
  vsem[0].sem_flg = SEM_UNDO;

  semop(semid, vsem, 1);
}

main(int argc, char *argv[])
{
  int *teste[8];
  int i;
  int semid;
  int pid, pid_father, pid_son;
  int com_semaforo;

  com_semaforo = 0;
  if (argc == 1) 
    com_semaforo = 1;

  printf("\nExecucao %s semaforo\n", (com_semaforo)? "com" : "sem");

  pid_father = getpid(); /* busca o n'umero do processo */
  semid = GetSem(13);

  if(semid < 0)
    {
      printf("\nSemaforo nao criado por GetSem !\n");
      exit(0);
    }

  for(i=0; i<8; i++)
    teste[i] = i;

  pid_son = fork();
  pid = (pid_son)? pid_son : pid_father;

  printf("Inicio de execucao do processo %d, semaforo %d\n", pid, semid);

  while(1)
  {
    if (com_semaforo)
      DOWN(semid); /*   entrada na Regiao Critica */
    for(i = 0; i < 8; i++)
    {
      printf("\n%d: Esse é o teste:%d", pid, teste[i]);
      sleep(0.5);
    }
    printf("\n\n");
    if (com_semaforo)
      UP(semid); /*  saida da Regiao Critica */
    sleep(2);
  }
}
