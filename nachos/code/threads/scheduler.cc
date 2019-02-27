// scheduler.cc 
//  Routines to choose the next thread to run, and to dispatch to
//  that thread.
//
//  These routines assume that interrupts are already disabled.
//  If interrupts are disabled, we can assume mutual exclusion
//  (since we are on a uniprocessor).
//
//  NOTE: We can't use Locks to provide mutual exclusion here, since
//  if we needed to wait for a lock, and the lock was busy, we would 
//  end up calling FindNextToRun(), and that would put us in an 
//  infinite loop.
//
//  Very simple implementation -- no priorities, straight FIFO.
//  Might need to be improved in later assignments.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "debug.h"
#include "scheduler.h"
#include "main.h"

//----------------------------------------------------------------------
// Scheduler::Scheduler
//  Initialize the list of ready but not running Process.
//  Initially, no ready threads.
//----------------------------------------------------------------------

Scheduler::Scheduler()
{ 
    //readyList = new List<Process *>; 
    readyList=  new SortedList<Process*>(Process::compare);
    toBeDestroyProcess = NULL;
} 

//Initialize the list of ready but not running threads.
Scheduler::Scheduler(int i)
{ 
    readyListT = new List<Thread *>; 
    toBeDestroyed = NULL;
}


//----------------------------------------------------------------------
// Scheduler::~Scheduler
//  De-allocate the list of ready threads.
//----------------------------------------------------------------------

Scheduler::~Scheduler()
{ 
    delete readyListT;
    delete readyList; 
} 

//----------------------------------------------------------------------
// Scheduler::ReadyToRunT
//  Mark a thread as ready, but not running.
//  Put it on the ready list, for later scheduling onto the CPU.
//
//  "thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void
Scheduler::ReadyToRunT (Thread *thread)
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    DEBUG(dbgThread, "Putting thread on ready list: " << thread->getName());

    thread->setStatus(READY);
    readyListT->Append(thread);
}

//ReadyToRun for process
void
Scheduler::ReadyToRun (Process *process)
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    DEBUG(dbgThread, "Putting Process on ready list: " << process->getName());

    process->setStatus(READY);
    process->currentThread->setStatus(READY);
    readyList->Insert(process);
}


//----------------------------------------------------------------------
// Scheduler::FindNextToRunT
//  Return the next thread to be scheduled onto the CPU.
//  If there are no ready threads, return NULL.
// Side effect:
//  Thread is removed from the ready list.
//----------------------------------------------------------------------
Thread *
Scheduler::FindNextToRunT ()
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (readyListT->IsEmpty()) {
    return NULL;
    } else {
        return readyListT->RemoveFront();
    }
}

//Find the next prcoess from the readyList
Process *
Scheduler::FindNextToRun ()
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (readyList->IsEmpty()) {
    return NULL;
    } else {
        return readyList->RemoveFront();
    }
}




//----------------------------------------------------------------------
// Scheduler::Run
//  Dispatch the CPU to nextThread.  Save the state of the old thread,
//  and load the state of the new thread, by calling the machine
//  dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//  already been changed from running to blocked or ready (depending).
// Side effect:
//  The global variable kernel->currentThread becomes nextThread.
//
//  "nextThread" is the thread to be put into the CPU.
//  "finishing" is set if the current thread is to be deleted
//      once we're no longer running on its stack
//      (when the next thread starts running)
//----------------------------------------------------------------------
void
Scheduler::RunT (Thread *nextThread, bool finishing)
{
    Thread *oldThread = kernel->currentProcess->currentThread;
    
    cout << "\nCurrent Thread: "<< oldThread->getName()<< endl ; 
    cout << "Next Thread: " << nextThread->getName() << endl;
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (finishing) {    // mark that we need to delete current thread
         ASSERT(toBeDestroyed == NULL);
     toBeDestroyed = oldThread;
    }

    if (oldThread->space != NULL) { // if this thread is a user program,
        oldThread->SaveUserState();     // save the user's CPU registers
    oldThread->space->SaveState();
    }
    
    oldThread->CheckOverflow();         // check if the old thread
                        // had an undetected stack overflow

    kernel->currentProcess->currentThread = nextThread;  // switch to the next thread
    nextThread->setStatus(RUNNING);      // nextThread is now running
    
    DEBUG(dbgThread, "Switching from: " <<kernel->currentProcess << " " << oldThread->getName() << " to: " <<  kernel->currentProcess << " "<< nextThread->getName());
    cout<<"Switching from: " <<kernel->currentProcess->getName() << " " << oldThread->getName() << " to: " <<  kernel->currentProcess->getName( ) << " "<< nextThread->getName()<< endl;
    nextThread->printThread();
    // This is a machine-dependent assembly language routine defined 
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".

    SWITCH(oldThread, nextThread);
   

    // we're back, running oldThread
      
    // interrupts are off when we return from switch!
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    DEBUG(dbgThread, "Now in thread: " << oldThread->getName());

    CheckToBeDestroyed();       // check if thread we were running
                    // before this one has finished
                    // and needs to be cleaned up
    
    if (oldThread->space != NULL) {     // if there is an address space
        oldThread->RestoreUserState();     // to restore, do it.
    oldThread->space->RestoreState();
    }
}

//Switch to the next process
void
Scheduler::Run (Process *nextProcess, bool finishing)
{
    Thread *oldThread = kernel->currentProcess->currentThread;
    Process *oldProcess = kernel->currentProcess;
    cout<< "------------------------------------\n";
    cout << "Current Process: "<<kernel->currentProcess->getName()<< endl ; 
    cout << "Next Process: " << nextProcess->getName() << endl;
    printf("------------------------------------\n"); 
    kernel->scheduler->Print();
    printf("\n------------------------------------\n"); 
 
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (finishing) {    // mark that we need to delete current thread
        toBeDestroyProcess = NULL;
        ASSERT(toBeDestroyProcess == NULL);
        toBeDestroyProcess = oldProcess;
    }
    if (oldThread->space != NULL) {   // if this thread is a user program,
        oldThread->SaveUserState();     // save the user's CPU registers
    oldThread->space->SaveState();
    }
    
    //oldThread->CheckOverflow();           // check if the old thread
                        // had an undetected stack overflow
   
    kernel->currentProcess = nextProcess;
    Thread *nextThread = nextProcess->currentThread;
 
    kernel->currentProcess->currentThread = nextThread;  // switch to the next thread
    nextProcess->currentThread->setStatus(RUNNING);      // nextThread is now running
    nextProcess->setStatus(RUNNING); 

    
    DEBUG(dbgThread, "Switching from: " << oldProcess->getName() <<" "<< oldThread->getName() << " to: " << nextProcess->getName() << " " << nextThread->getName());
    cout <<"Process Switching from: " << oldProcess->getName() <<" "<< oldThread->getName() << " to: " << nextProcess->getName() << " " << nextThread->getName() << endl; 
    nextProcess->printProcess();
    nextProcess->currentThread->printThread();
    // This is a machine-dependent assembly language routine defined 
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".
     SWITCH(oldThread, nextThread);

    // we're back, running oldThread
      
    // interrupts are off when we return from switch!
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    DEBUG(dbgThread, "Now in thread: " << oldThread->getName());

    CheckToBeDestroyedProcess();
    
    if (oldThread->space != NULL) {       // if there is an address space
        oldThread->RestoreUserState();     // to restore, do it.
    oldThread->space->RestoreState();
    }
}



//----------------------------------------------------------------------
// Scheduler::CheckToBeDestroyed
//  If the old thread gave up the processor because it was finishing,
//  we need to delete its carcass.  Note we cannot delete the thread
//  before now (for example, in Thread::Finish()), because up to this
//  point, we were still running on the old thread's stack!
//----------------------------------------------------------------------

void
Scheduler::CheckToBeDestroyed()
{
    if (toBeDestroyed != NULL) {
        delete toBeDestroyed;
    toBeDestroyed = NULL;
    }
}


//Check the prcoess to be destroyed and delete it
void
Scheduler::CheckToBeDestroyedProcess()
{
    if (toBeDestroyProcess != NULL) {
        delete toBeDestroyProcess;
    toBeDestroyProcess = NULL;
    }
}
 
//----------------------------------------------------------------------
// Scheduler::Print
//  Print the scheduler state -- in other words, the contents of
//  the ready list.  For debugging.
//----------------------------------------------------------------------
void
Scheduler::Print()
{
    cout << "Ready list contents:\n";
    readyList->Apply(ProcessPrint);
}


//Print the thread List
void
Scheduler::PrintThread()
{
    cout << "Ready list Thread contents:\n";
    readyListT->Apply(ThreadPrint);
}