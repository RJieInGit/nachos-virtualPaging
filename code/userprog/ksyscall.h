/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"




void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

void SysWrite(int buffer, int size){
  printf("here");
  int content;
  for(int i=0;i<size;i++){
    if(!kernel->machine->ReadMem(buffer,1,&content))
    break;
    buffer++;
    printf("%c",content);
  }
  printf("sys_write called finished here \n");
   kernel->stats->Print(); 
}

void SysRead(int buffer,int size, char* content){
  int c;
  for(int i=0;i>size;i++){
    c=(int)content[i];
    if(!kernel->machine->ReadMem(buffer,1,&c))
    break;
  }
  printf("sys_read called finished here \n");
   kernel->stats->Print(); 
}

void SysExit(){
  delete kernel->currentThread->space;
  printf("exit called here \n");
   kernel->stats->Print(); 
  kernel->currentThread->Finish();
}

void usrFork(int arg){
  kernel->currentThread->RestoreUserState();
  kernel->machine->Run();
}
int SysFork(){
  int funcaddr= kernel->machine->ReadRegister(4);
   Thread *t = new Thread("SC_forked thread");
   t->space = new AddrSpace(kernel->currentThread->space);
   t->SaveUserState();
   t->setReg(PCReg,funcaddr);
   t->setReg(NextPCReg,funcaddr+4);
   t->Fork((VoidFunctionPtr)usrFork,(void *)0);
printf("sys_fork called finished here \n");
   kernel->stats->Print(); 
    return t->space->spaceId;
}

void execPro(AddrSpace *space){
  ASSERT(space != (AddrSpace *)NULL);
  space->Execute();
}

int  SysExec(int filename){
  int size=100;
  int addr = filename;
  char* name = new char[100];
  for(int i=0;i<size;i++){
    int c;
    if(!kernel->machine->ReadMem(addr,size,&c))
      break;
      name[i]=c;
  }
   AddrSpace *space = new AddrSpace;
   space->Load(name);
   Thread *th = new Thread("execthread");
   th->Fork((VoidFunctionPtr)execPro,(void *)space);
   printf("exec called finished here \n");
   kernel->stats->Print(); 
   return 0;
}


#endif /* ! __USERPROG_KSYSCALL_H__ */
