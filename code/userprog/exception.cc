// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
			case PageFaultException:{
	/* Modify return point */
	{
		printf("pagefault\n");
		int faultaddr = kernel->machine->ReadRegister(BadVAddrReg);
		int faultpn=faultaddr/PageSize;
	    int ppn= kernel->freeMap->FindAndSet();
		TranslationEntry* pageEntry = kernel->currentThread->space->getPageEntry(faultpn);
		printf("faultpn: %d , vpn : %d, ppn: %d \n",faultpn,pageEntry->virtualPage, ppn);
		if(ppn!=-1){
			pageEntry->physicalPage=ppn;
			pageEntry->valid=true;
			kernel->swapspace->ReadAt(&(kernel->machine->mainMemory[ppn*PageSize]),PageSize,PageSize*pageEntry->virtualPage);
			kernel->entryList->Append(pageEntry);
		}
		else{
			//the entry at the front is the least recent used entry
			TranslationEntry *evict= kernel->entryList->RemoveFront();
			int ppn = evict->physicalPage;
			if(evict->dirty){

			}
			evict->physicalPage=-1;
			evict->valid=false;
			pageEntry->physicalPage=ppn;
			pageEntry->valid=true;
			kernel->swapspace->ReadAt(&(kernel->machine->mainMemory[ppn*PageSize]),PageSize,PageSize*pageEntry->virtualPage);
			kernel->entryList->Append(pageEntry);
		}
	  printf("%s\n",kernel->machine->mainMemory);
	}

	return;
	
	ASSERTNOTREACHED();
	}
	break;
	
    case SyscallException:
      switch(type) {
      case SC_Halt:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

	SysHalt();

	ASSERTNOTREACHED();
	break;

      case SC_Add:
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	
	ASSERTNOTREACHED();

	break;
 
 // syscall read
	case  SC_Read:{
	/* Modify return point */
	
	  /* set previous programm counter (debugging only)*/
	  char *con = "simulate read content";
	  int buffer = kernel->machine->ReadRegister(4);
	  int size = kernel->machine->ReadRegister(5);
	  SysRead(buffer,size,con);
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	

	return;
	
	ASSERTNOTREACHED();
	}
	break;
	//syscall write 
	case SC_Write:{
	
	DEBUG(dbgSys, "Write " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
    int buffer = kernel->machine->ReadRegister(4);
	int size =kernel->machine->ReadRegister(5);
	SysWrite(buffer,size);
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	
	ASSERTNOTREACHED();
}
	break;
	
	//syscall exec
	case SC_Exec:{
	int filename = kernel->machine->ReadRegister(4);
	int sid = SysExec(filename);
	kernel->machine->WriteRegister(2,sid);

	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	
	ASSERTNOTREACHED();
}
	break;

	//syscall exit
	case SC_Exit:{
	/* Modify return point */
	{
	  SysExit();
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	
	ASSERTNOTREACHED();
}
	break;
    //syscall fork
	case SC_ThreadFork:{
	/* Modify return point */
	{

	  int sid =SysFork();
	  kernel->machine->WriteRegister(2,sid);
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);

	  kernel->machine->WriteRegister(2,sid);
	}

	return;
	
	ASSERTNOTREACHED();
	}
	break;

      default:
	cerr << "Unexpected system call " << type << "\n";
	break;
      }
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
    ASSERTNOTREACHED();
}
