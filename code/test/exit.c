#include "syscall.h"

int
main()
{
  printf("syscall exit called here\n");
  Threadexit(0);
  /* not reached */
}