#include "syscall.h"

int
main()
{
  char* buffer= "test write syscall";
  Write(buffer,50,0);

  /* not reached */
}