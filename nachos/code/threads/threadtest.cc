#include "kernel.h"
#include "main.h"

//Test Function
void
SimpleThread(int which)
{
    int num;  
    printf("In function SimpleThread\n");
    kernel->currentProcess->currentThread->Yield();
    kernel->currentProcess->Terminate();
} 
//Test Function
void fun1(int which){
    cout << "\nIn function 1 \n";
    kernel->currentProcess->currentThread->Yield();
    kernel->currentProcess->Terminate();    
}

//Runs the testcases
void 
ThreadTest()
{
    cout<< "\nProcess1 Created \n";
    Process *p1 = new Process("Process1",1);
    p1->printProcess();
    cout << "Thread Forked for Process2\n";
    p1->Fork((VoidFunctionPtr) fun1, (void *) 1);
    p1->printProcess();
    cout<< "Create ChildProcess for Process1 (Process Fork)\n";
    p1->createChildProcess("Child Process1");
    cout<< "Process2 Created \n";
    Process *p2 = new Process("Process2",2);
    p2->printProcess();
    cout << "Thread Forked for Process2\n";
    p2->Fork((VoidFunctionPtr) SimpleThread, (void *) 1);
    p2->printProcess();
    cout << "Thread Forked for Process2\n";
    Thread *t = new Thread("Thread2");
    t->Fork((VoidFunctionPtr) SimpleThread, (void *) 1, p2);
    t->printThread();
    Thread *t1 = new Thread("Thread3");
    t1->Fork((VoidFunctionPtr) fun1, (void *) 1, p2);
    t1->printThread();
    cout<< "Process2 Created \n";
    Process *p3 = new Process("Process3",3);
    p3->printProcess();
    cout << "Thread Forked for Process3\n";
    p3->Fork((VoidFunctionPtr) SimpleThread, (void *) 1);
    p3->printProcess();
   

	kernel->currentProcess->Yield();
}
