/* Process.h
Date structures for managing the Prcoess.
*/
#ifndef PROCESS_H
#define PROCESS_H

#include "list.h"
#include "thread.h"


class Scheduler;

class Process {

  public:
  Process(char* debugName, int pr);		// initialize a Process 
  ~Process(); 				// deallocate a Process

  Thread *currentThread;
  Scheduler *threadScheduler;

  void setStatus(ThreadStatus st);
	void setPriority(int p);
  void setPId(int i);
	void setName(char* c);
  void setFuncPtr(VoidFunctionPtr f);
  void setArg(void *a);

	ThreadStatus getStatus();
	int getPriority();
  int getPId();
	char* getName();
  VoidFunctionPtr getFUncPtr();
  void* getArg();

  void Print() { cout << name << "  " ; }
  void printProcess();
  char* printStatus();
  static int compare(Process* p1, Process* p2);
  Process* createChildProcess(char* n);
  void Fork(VoidFunctionPtr func, void *arg); 
  void Yield();      // Relinquish the CPU if any other Process is runnable
  void Terminate();  
	void Sleep(bool finishing); 
  
  private:
  
    ThreadStatus status;	// ready, running or blocked
    char* name;
    int priority;
    int pId;
    VoidFunctionPtr funcPtr;
    static int count;
    void* arg;
}; 

extern void ProcessPrint(Process *process);  

#endif // PROCESS_H
