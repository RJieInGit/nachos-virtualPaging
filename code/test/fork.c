#include "syscall.h"

void prog(void *arg){
    char* buffer = "forked by syscall \n";
    Write(buffer,50,0);
}

int
main()
{
  printf("now  test syscall fork\n");
  ThreadFork(prog);
  /* not reached */
}