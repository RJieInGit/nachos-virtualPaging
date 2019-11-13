#include "syscall.h"
#include "stdio.h"
int
main()
{
  printf("now test exec syscall by execute exit usrprog\n");
  Exec("../test/exit");
  
  /* not reached */
}