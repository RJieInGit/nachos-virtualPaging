#include "syscall.h"

int
main()
{
  char* buffer= "test write syscall";
  Write(buffer,50);

  printf("write syscall finish\n");
  /* not reached */
}