#include "syscall.h"

void prog(void *arg){
    Write("forked by syscall \n");
}

int
main()
{
  printf("now  test syscall fork\n");
  ThreadFork(prog);
  /* not reached */
}