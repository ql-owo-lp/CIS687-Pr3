///////////////////////////////////////////////////////////////
// Threads.cpp - Create and run threads                      //
// ver 1.1                                                   //
// Language: standard C++                                    //
// Platform: Dell Dimension T7400, Windows 7, SP #1          //
// Application: Resource for DO projects                     //
// Author:      Jim Fawcett, CST 4-187, Syracuse University  //
//              jfawcett@twcny.rr.com, (315) 443-3948        //
///////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include "Threads.h"

//----< test stub >--------------------------------------------

#ifdef TEST_THREADS

///////////////////////////////////////////////////////////////
// demonstration derived class

class ThreadDemo : public threadBase
{
public:
  ThreadDemo(std::string& Message);
private:
  gCSLock<1> tLock;
  virtual void run();
  std::string& Msg;
};

//----< demo thread constructor >------------------------------

ThreadDemo::ThreadDemo(std::string& Message) : Msg(Message) {}

//----< here is where what you want to happen happens >--------

void ThreadDemo::run()
{
  for(size_t i=0; i<25; ++i)
  {
    tLock.lock();
    Msg += '0';
    tLock.unlock();
    ::Sleep(2);
  }
  sout << "\n  child exiting";
}

class TThreadDemo : public tthreadBase
{
public:
  TThreadDemo(std::string& Message);
private:
  gCSLock<1> tLock;
  virtual void run();
  std::string& Msg;
};

//----< demo thread constructor >------------------------------

TThreadDemo::TThreadDemo(std::string& Message) : Msg(Message) {}

//----< here is where what you want to happen happens >--------

void TThreadDemo::run()
{
  for(size_t i=0; i<25; ++i)
  {
    tLock.lock();
    Msg += '0';
    tLock.unlock();
    ::Sleep(2);
  }
  sout << "\n  child exiting";
}
//----< test stub >--------------------------------------------

int main()
{
  std::cout << "\n  Demonstrating Basic Thread Processing in Thread Class";
  std::cout << "\n =======================================================\n";

  std::string Shared;

  {
    ThreadDemo td(Shared);
    td.start();
    gCSLock<1> pLock;
    for(size_t i=0; i<25; ++i)
    {
      pLock.lock();
      Shared += '1';
      pLock.unlock();
      ::Sleep(1);
    }
    td.join();
  }
  sout << locker << "\n  " << Shared.c_str() << unlocker;
  std::cout << std::endl;

  Shared = "";
  {
    TThreadDemo* pTD = new TThreadDemo(Shared);
    pTD->start();
    gCSLock<1> pLock;
    for(size_t i=0; i<25; ++i)
    {
      pLock.lock();
      Shared += '1';
      pLock.unlock();
      ::Sleep(1);
    }
    pTD->join();
    // note: don't call pTD->delete();
  }
  sout << locker << "\n  " << Shared.c_str() << unlocker;
  sout.flush();
  sout << "\n\n  parent exiting\n\n";
}

#endif
