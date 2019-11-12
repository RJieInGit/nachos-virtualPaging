#include "syscall.h"

int
main()
{
  char*  buffer = new char[50]

  Read(buffer,50,0);
  printf("Syscall read: the content read to buffer is %s\n",buffer);
  /* not reached */
}