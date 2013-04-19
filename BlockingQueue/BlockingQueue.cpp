/////////////////////////////////////////////////////////////////////
// BlockingQueue.cpp - Thread-safe queue that blocks on empty deQ  //
// ver 1.1                                                         //
// Language: standard C++                                          //
// Platform: Dell Dimension T7400, Windows 7, SP #1                //
// Application: Resource for DO projects                           //
// Author:      Jim Fawcett, CST 4-187, Syracuse University        //
//              jfawcett@twcny.rr.com, (315) 443-3948              //
/////////////////////////////////////////////////////////////////////

#ifdef TEST_BLOCKINGQUEUE

#include "BlockingQueue.h"
#include "../NewThreadsAndLocks/Threads.h"
#include <string>
#include <iostream>
#include <sstream>

//----< test stub >--------------------------------------------

///////////////////////////////////////////////////////////////
// demonstration derived class

class TestQThread : public ThreadBase<DefaultTerminate>
{
public:
  TestQThread(BlockingQueue<std::string>& q);
private:
  BlockingQueue<std::string>& _Q;  // use creator's queue
  virtual void run();
};

//----< demo thread constructor >------------------------------

TestQThread::TestQThread(BlockingQueue<std::string>& q) : _Q(q) {}

//----< here is where what you want to happen happens >--------

void TestQThread::run()
{
  std::string msg;
  gCSLock<1> IOlock;    // global lock shares a static CS
  do
  {
    msg = _Q.deQ();
    IOlock.lock();
    std::cout << "\n  deQ'd message: " << msg.c_str();
    IOlock.unlock();
    ::Sleep(10);
  }
  while(msg != "quit");
  std::cout << "\n  child exiting";
}
//----< test stub >--------------------------------------------

int main()
{
  std::cout << "\n  Demonstrating BlockingQueue<std::string> Operation";
  std::cout << "\n ====================================================\n";

  BlockingQueue<std::string> q;
  TestQThread td(q);
  td.start();

  gCSLock<1> IOlock;  // global lock shares a static CS
  std::string msg;
  for(size_t i=0; i<25; ++i)
  {
    std::ostringstream convert;  // convert size_t value to string
    convert << i;
    msg = std::string("Msg #") + convert.str();
    q.enQ(msg);
    IOlock.lock();
    std::cout << "\n  main thread enQ'd " << msg.c_str();
    IOlock.unlock();
    ::Sleep(5);
  }
  q.enQ("quit");
  td.join();
  std::cout << "\n  parent exiting\n\n";
}


#endif
