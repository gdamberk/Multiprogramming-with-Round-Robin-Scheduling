//process.cc
//Routine to manage the process

#include "process.h"
#include "switch.h"
#include "synch.h"

int Process::count = 0;
//Constructor for Process
Process::Process(char* processName, int priority_)
{
    name = processName;
    status = JUST_CREATED;
    priority = priority_;
    threadScheduler = new Scheduler(1); 
    currentThread  = new Thread("Thread1");
    pId = count++;
}
//Destructor for process
Process::~Process()
{
    
}

//setter for status
void Process::setStatus(ThreadStatus st){
    status = st;
}

//setter for Priority
void Process::setPriority(int p){
    priority = p;
}

//setter for PId
void Process::setPId(int i){
    pId = i;
}

//setter for Name
void Process::setName(char* c){
    name = c;
}

 //setter for funcPtr
void Process::setFuncPtr(VoidFunctionPtr f){
    funcPtr = f;
}

//setter for arg
void Process::setArg(void *a){
    arg = a;
}

//getter for status
ThreadStatus Process::getStatus(){
    return status;
}

//getter for Priority
int Process::getPriority(){
    return priority;
}

//getter for PId
int Process::getPId(){
    return pId;
}

//getter for name
char* Process::getName(){
    return name;
}

//getter for funcPtr
VoidFunctionPtr Process::getFUncPtr(){
    return funcPtr;
}

//getter for arg
void* Process::getArg(){
    return arg;
}
//print the status of process
char* Process::printStatus(){
    switch(status){
        case 0: return "JUST_CREATED";
                break;
        case 1: return "RUNNING";
                break;
        case 2: return "READY";
                break;
        case 3: return "BLOCKED";
                break;
        default: return "NOT DEFINED";
    }
}
//This function is used while inserting the process in sorted list
//This function comapres Priorities of process. 
int Process::compare(Process* p1, Process* p2){

    int p1Priority = p1->getPriority();
    int p2Priority = p2->getPriority();

    if(p1Priority > p2Priority)
        return -1;
    if(p1Priority == p2Priority)
        return 0;
    else
        return 1;
}

//This method forks the thread in given process
void Process::Fork(VoidFunctionPtr func, void *arg)
{
    Interrupt *interrupt = kernel->interrupt;
    Scheduler *scheduler = kernel->scheduler;
    IntStatus oldLevel;
    this->setFuncPtr(func);
    this->setArg(arg);
  
    DEBUG(dbgThread, "Forking Process: " << name << " f(a): " << (int) func << " " << arg);
    
    oldLevel = interrupt->SetLevel(IntOff);
    currentThread->Fork(func, arg, this);
    scheduler->ReadyToRun(this);    // ReadyToRun assumes that interrupts are disabled!
    (void) interrupt->SetLevel(oldLevel);   
}

//This methos yields the process
void Process::Yield ()
{
    Process *nextProcess;
    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
    
    ASSERT(this == kernel->currentProcess);
    
    DEBUG(dbgThread, "Yielding Process: " << name);
    cout <<  "Yielding Process: " << name<< endl;
    
    nextProcess= kernel->scheduler->FindNextToRun();
    if (nextProcess != NULL) {
    kernel->scheduler->ReadyToRun(this);
    kernel->scheduler->Run(nextProcess, FALSE);
    }
    (void) kernel->interrupt->SetLevel(oldLevel);
}

//Print the Process details
void ProcessPrint(Process *t) { t->Print(); }

//Terminate the Process 
void Process::Terminate ()
{
    Thread *nextThread;
    (void) kernel->interrupt->SetLevel(IntOff);     
    ASSERT(this == kernel->currentProcess);
    bool finished = TRUE;

    Thread *cThread = kernel->currentProcess->currentThread;
    cout << "Finishing Thread: " << cThread->getName() << endl;
    if(finished){
        cout << "Terminating Process: " << name << endl;
        kernel->currentProcess->Sleep(TRUE);
    }
   
}
//Put the process on SLeep
void Process::Sleep (bool finishing)
{
    Process *nextProcess;
    
    ASSERT(this == kernel->currentProcess);
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    
    DEBUG(dbgThread, "Sleeping Process: " << name);

    status = BLOCKED;
    while ((nextProcess = kernel->scheduler->FindNextToRun()) == NULL)
    kernel->interrupt->Idle();  // no one to run, wait for an interrupt
    // returns when it's time for us to run
    kernel->scheduler->Run(nextProcess, finishing); 
}

//Create the child process
Process* Process::createChildProcess(char* cName){
     Process *p = new Process(cName,this->getPriority());
     p->Fork(this->getFUncPtr(),this->getArg());
     p->printProcess();
}

//Print the process details
void Process::printProcess(){
    cout << "------------------------------------\nProcess Details:\n"; 
    cout << "PId: "<< this->getPId() << ", Name: "<< this->getName() ;
    cout << ", Priority: " << this->getPriority();
    cout << ", Status: " << this->printStatus() << endl;
}