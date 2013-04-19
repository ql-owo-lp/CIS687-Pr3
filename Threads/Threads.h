#ifndef THREADS_H
#define THREADS_H
///////////////////////////////////////////////////////////////
// Threads.h - Create and run threads                        //
// ver 1.1                                                   //
// Language: standard C++                                    //
// Platform: Dell Dimension T7400, Windows 7, SP #1          //
// Application: Resource for DO projects                     //
// Author:      Jim Fawcett, CST 4-187, Syracuse University  //
//              jfawcett@twcny.rr.com, (315) 443-3948        //
///////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * ===================
 * Two classes are defined in this package, a base class ThreadBase
 * and a demonstraton derived class ThreadDemo.  If a the child thread
 * fails to start, a std::exception object will be thrown, which client
 * code should be prepared to catch.
 */
/*
 * Public Interface:
 * =================
 * ThreadBase* th = new someThreadClass;
 * th.start();	// start thread
 * th.join();	// join thread
 *
 * Required Files:
 * ---------------
 * Thread.h, Thread.cpp, Locks.h, Locks.cpp
 *
 * Build Process:
 * --------------
 * cl /EHa /DTEST_THREAD Thread.cpp
 *
 * Maintenance History:
 * --------------------
 * ver 1.1 : 24 Mar 13
 * - removed locks code, now using locks package
 * ver 1.0 : 19 Feb 12
 * - first release
 */

#include <Windows.h>
#include <process.h>
#include <exception>
#include "Locks.h"

struct DefaultTerminate
{
  DefaultTerminate() 
  { 
    doLog("constructing DefaultTerminate"); 
  }
  bool selfTerminate() { return false; }
  virtual ~DefaultTerminate() 
  {
    doLog("destroying DefaultTerminate");
  }
};

struct SelfTerminate
{
  SelfTerminate()
  {
    doLog("constructing SelfTerminate");
  }
  bool selfTerminate() { return true; }
  virtual ~SelfTerminate() 
  {
    doLog("destroying SelfTerminate");
  }
};
///////////////////////////////////////////////////////////////
// base thread class

template <typename TerminatePolicy>
class ThreadBase : public TerminatePolicy
{
public:
  ThreadBase();
  virtual ~ThreadBase();
  void start();
  void join();
protected:
  ::HANDLE hThread;
  unsigned int _threadID;
private:
  virtual void run()=0;
  static unsigned int __stdcall threadOps(void* pArg);
};

typedef ThreadBase<DefaultTerminate> threadBase;
typedef ThreadBase<SelfTerminate> tthreadBase;

template <typename TerminatePolicy>
inline ThreadBase<TerminatePolicy>::~ThreadBase() 
{
  doLog("destroying ThreadBase");
  CloseHandle(hThread); 
}

//----< ThreadBase constructor >-------------------------------

template <typename TerminatePolicy>
ThreadBase<TerminatePolicy>::ThreadBase()
{
  doLog("constructing ThreadBase");
  hThread = (HANDLE)_beginthreadex
            (
              NULL,             // default security properties
              0,                // default stack size
              threadOps,        // function that thread runs
              (void*)this,      // static function can access thread members
              CREATE_SUSPENDED, // start thread suspended, call start() to run
              &_threadID        // OS thread identifier
              );
  if(hThread == 0)
    throw std::exception("\n  failed to create thread");
}
//----< start thread running >---------------------------------

template <typename TerminatePolicy>
void ThreadBase<TerminatePolicy>::start()
{
  doLog("starting child thread");
  ResumeThread(hThread);
}
//----< this is where the derived processing gets to run >-----

template <typename TerminatePolicy>
unsigned int ThreadBase<TerminatePolicy>::threadOps(void* pThis)
{
  doLog("in threadOps");
  ((ThreadBase<TerminatePolicy>*)pThis)->run();
  if(((ThreadBase<TerminatePolicy>*)pThis)->selfTerminate())
    delete static_cast<TerminatePolicy*>(pThis);
  return 0;
}
//----< wait for child thread to exit >------------------------

template <typename TerminatePolicy>
void ThreadBase<TerminatePolicy>::join()
{
  doLog("waiting for thread exit");
  ::WaitForSingleObject(hThread,INFINITE);
  doLog("wait over - thread exited");
}

#endif
