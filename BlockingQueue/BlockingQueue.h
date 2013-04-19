#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H
///////////////////////////////////////////////////////////////////
// BlockingQueue.h - Thread-safe queue that blocks on empty deQ  //
// ver 1.1                                                       //
// Language: standard C++                                        //
// Platform: Dell Dimension T7400, Windows 7, SP #1              //
// Application: Resource for DO projects                         //
// Author:      Jim Fawcett, CST 4-187, Syracuse University      //
//              jfawcett@twcny.rr.com, (315) 443-3948            //
///////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * ===================
 * This package contains a BlockingQueue<> class that is thread-safe
 * and blocks a deQ'er if the queue is empty.  Blocking is accomplished
 * using a ConditionVariable (introduced with Vista) and an associated
 * lock based on a CriticalSection.
 *
 * Users don't need to be aware of how this works.  They just use
 * the queue without worrying about locking.
 */
/*
 * Public Interface:
 * =================
 * BlockingQueue<int> b;
 * int i = b.deQ();	// de-queue from blocking queue
 * b.enQ(2);	// en-queue to blocking queue
 *
 * Required Files:
 * ---------------
 * BlockingQueue.h, BlockingQueue.cpp, Locks.h, Locks.cpp, Thread.h, Thread.cpp
 *
 * Build Process:
 * --------------
 * cl /EHa /DTEST_BLOCKINGQUEUE BlockingQueue.cpp Locks.cpp Thread.cpp
 *
 * Maintenance History:
 * --------------------
 * ver 1.1 : 24 Mar 13
 * - small revisions for new threadBase
 * ver 1.0 : 19 Feb 12
 * - first release
 */

#include <queue>
#include <Windows.h>
#include "../Threads/locks.h"

///////////////////////////////////////////////////////////////
// BlockingQueue<Msg>

template <typename Msg>
class BlockingQueue
{
public:
  BlockingQueue();
  void enQ(Msg msg);
  Msg deQ();
  size_t size();
private:
  std::queue<Msg> _Q;
  CSLock qLock;
  CSConditionVariable qCv;
};
//----< Ctor >-------------------------------------------------

template <typename Msg>
BlockingQueue<Msg>::BlockingQueue()
{
  qCv.wakeAll();  // clear lock - probably not necessary
}
//----< add a message to queue >-------------------------------

template <typename Msg>
void BlockingQueue<Msg>::enQ(Msg msg)
{
  qLock.lock();
  _Q.push(msg);
  qLock.unlock();
  qCv.wake();
}
//----< remove a message from queue >--------------------------

template <typename Msg>
Msg BlockingQueue<Msg>::deQ()
{
  Msg msg;
  qLock.lock();
  if(_Q.size() > 0)
  {
    msg = _Q.front();
    _Q.pop();
    qLock.unlock();
    return msg;
  }
  // I'm not sure if this loop is necessary

  while(true)
  {
    qCv.sleep(qLock);
    if(_Q.size() == 0)
      continue;
    msg = _Q.front();
    _Q.pop();
    qLock.unlock();
    return msg;
  }
}
//----< return number of queueud messages >--------------------

template <typename Msg>
size_t BlockingQueue<Msg>::size()
{
  qLock.lock();
  size_t sz = _Q.size();
  qLock.unlock();
  return sz;
}

#endif
