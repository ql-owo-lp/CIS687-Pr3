///////////////////////////////////////////////////////////////
// Locks.h - Define Lock classes based on:                   //
//             CriticalSection, SlimReaderWriter, Mutex      // 
// ver 1.1                                                   //
// Language: standard C++                                    //
// Platform: Dell Dimension T7400, Windows 7, SP #1          //
// Application: Resource for DO projects                     //
// Author:      Jim Fawcett, CST 4-187, Syracuse University  //
//              jfawcett@twcny.rr.com, (315) 443-3948        //
///////////////////////////////////////////////////////////////

#include "Locks.h"
#include <iostream>

//----< globally available output stream >---------------------------

syncOut sout(std::cout);  // globally available sout object
                          // mimics cout

#ifdef TEST_LOCKS

#include "Threads.h"
#include "../NewThreadsAndLocks/HiResTimer.h"

///////////////////////////////////////////////////////////////
// test derived class

class TestThread : public ThreadBase<DefaultTerminate>
{
public:
  TestThread(std::string& Message);
private:
  //gMLock<1> tLock;        // test Mutex-based lock
  gCSLock<1> tLock;       // test CriticalSection-based lock
  virtual void run();
  std::string& Msg;
};

//----< demo thread constructor >------------------------------

TestThread::TestThread(std::string& Message) : Msg(Message) {}

//----< here is where what you want to happen happens >--------

void TestThread::run()
{
  tLock.lock();
  doLog(Msg.c_str());
  std::cout.flush();
  Msg = "";
  tLock.unlock();
  doLog("hello back from child");
  for(size_t i=0; i<25; ++i)
  {
    tLock.lock();
    Msg += '0';
    tLock.unlock();
    ::Sleep(2);
  }
  doLog("child exiting");
}
//----< test stub >--------------------------------------------

int main()
{
  std::cout << "\n  Demonstrating Lock Processing";
  std::cout << "\n ===============================\n";

  HRTimer::HiResTimer timer;
  std::string Message = "hello from parent";
  {
    TestThread td(Message);
    timer.Start();
    td.start();
    //gMLock<1> pLock;              // test Mutex-based lock
    gCSLock<1> pLock;             // test CriticalSection-based lock
    for(size_t i=0; i<25; ++i)
    {
      pLock.lock();
      Message += '1';
      pLock.unlock();
      ::Sleep(1);
    }
    td.join();
    timer.Stop();
  }
  std::cout << "\n  This string was shared and concurrently modified:";
  std::cout << "\n  " << Message.c_str();
  std::cout << "\n  Elapsed time = " << timer.ElapsedMicroseconds() << " microsecs";
  std::cout.flush();
  std::cout << "\n  parent exiting\n\n";
}

#endif
