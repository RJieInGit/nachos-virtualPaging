#include "syscall.h"

void prog(void *arg){
    char* buffer = "forked by syscall \n";
    Write(buffer,50,0);
}

int
main()
{
  ThreadFork(prog);
  /* not reached */
}