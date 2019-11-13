#include "syscall.h"
#include "stdio.h“
void prog(void *arg){
    printf("forked prog by fork syscall");
}

int
main()
{
  printf("now  test syscall fork\n");
  ThreadFork(prog);
  /* not reached */
}