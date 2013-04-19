#ifndef LOCKS_H
#define LOCKS_H
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
/*
 * Package Operations:
 * ===================
 * Several types of lock implementations are provided based on:
 *   CriticalSections, Mutexes, and SlimReaderWriters
 * Locks based on CriticalSections and Mutexes are provided in
 * local and global forms.  The global locks use static 
 * CriticalSections or Mutexes so can be shared in different parts 
 * of a package, simply by declaring an instance of the lock in
 * both places.
 *
 * A SRWLock class is defined, but hasn't been tested yet.
 *
 * Also, you will find a ConditionVariable class implementation,
 * used to build a blocking queue.  See the note where the
 * ConditionVariable is defined, at the end of this file.
 */
/*
 * Public Interface:
 * =================
 * CSLock l;
 * l.lock();	// lock
 * l.unlock();	// unlock
 *
 * Required Files:
 * ---------------
 * Locks.h, Locks.cpp
 * (Thread.h, Thread.cpp are needed to run the test stub)
 *
 * Build Process:
 * --------------
 * cl /EHa /DTEST_THREAD Locks.cpp Thread.cpp
 *
 * Maintenance History:
 * --------------------
 * ver 1.1 : 24 Mar 2013
 * - added sout, moved doLog here, uses latest threadBase
 * ver 1.0 : 20 Feb 2012
 * - first release
 */

#include <Windows.h>
#include <exception>
#include <iostream>

///////////////////////////////////////////////////////////////
// CSLock - local lock class based on Win32 CRITICAL_SECTION

class CSLock
{
public:
  CSLock();
  ~CSLock();
  void lock();
  void unlock();
  operator ::CRITICAL_SECTION* ();  // cast operator used with CV
private:
  ::CRITICAL_SECTION cs;
};

inline CSLock::CSLock()
{
  ::InitializeCriticalSection(&cs);
}

inline CSLock::~CSLock() 
{ 
  ::DeleteCriticalSection(&cs); 
}

inline void CSLock::lock() 
{ 
  ::EnterCriticalSection(&cs);
}

inline void CSLock::unlock()
{
  ::LeaveCriticalSection(&cs);
}

inline CSLock::operator ::CRITICAL_SECTION* () { return &cs; }

///////////////////////////////////////////////////////////////
// gCSLock - global lock class based on Win32 CRITICAL_SECTION

template<int i>
class gCSLock
{
public:
  gCSLock();
  ~gCSLock();
  void lock();
  void unlock();
  operator ::CRITICAL_SECTION* ();  // cast operator used with CV
private:
  static ::CRITICAL_SECTION cs;
  static unsigned int refCount;
};

//----< statics are only initialized by first caller >---------

template<int i>
::CRITICAL_SECTION gCSLock<i>::cs;
template<int i>
unsigned int gCSLock<i>::refCount = 0;

template<int i>
gCSLock<i>::gCSLock()
{
  if(refCount==0)
  {
    if(i != -1) // doLog uses sout which uses gCSLock<-1>
      doLog("Initializing CriticalSection");
    ::InitializeCriticalSection(&cs);
  }
  ::InterlockedIncrement(&refCount);
}

template<int i>
gCSLock<i>::~gCSLock() 
{ 
  if(::InterlockedDecrement(&refCount) == 0)
  {
    ::DeleteCriticalSection(&cs);
    if(i != -1) // doLog uses sout which uses gCSLock<-1>
      doLog("Deleting CriticalSection");
  }
}

template<int i>
void gCSLock<i>::lock() 
{ 
  ::EnterCriticalSection(&cs);
}

template<int i>
void gCSLock<i>::unlock()
{
  ::LeaveCriticalSection(&cs);
}

template<int i>
inline gCSLock<i>::operator ::CRITICAL_SECTION* () { return &cs; }

///////////////////////////////////////////////////////////////
// MLock - local lock class based on Win32 Mutex

class MLock
{
public:
  MLock();
  ~MLock();
  void lock();
  void unlock();
private:
  ::HANDLE hMutex;
};

inline MLock::MLock()
{
  hMutex = CreateMutexA
           (
             NULL,    // security attributes
             false,   // won't start locked
             NULL     // no name, so create anonymous mutex
           );
  if(hMutex == NULL)
    throw std::exception("mutex creation failed");
}

inline MLock::~MLock() { CloseHandle(hMutex); }

inline void MLock::lock() 
{ 
  ::WaitForSingleObject(hMutex, INFINITE);
}

inline void MLock::unlock()
{
  ReleaseMutex(hMutex);
}

///////////////////////////////////////////////////////////////
// gMLock - global lock class based on Win32 Mutex

template<int i>
class gMLock
{
public:
  gMLock();
  ~gMLock();
  void lock();
  void unlock();
private:
  static ::HANDLE hMutex;
  static unsigned int refCount;
};

//----< statics are only initialized by first caller >---------

template<int i>
::HANDLE gMLock<i>::hMutex;
template<int i>
unsigned int gMLock<i>::refCount = 0;

template<int i>
gMLock<i>::gMLock()
{
  if(refCount == 0)
  {
    hMutex = CreateMutexA
             (
               NULL,    // security attributes
               false,   // won't start locked
               NULL     // no name, so create anonymous mutex
             );
    if(hMutex == NULL)
      throw std::exception("mutex creation failed");
    doLog("Creating global mutex");
  }
  ::InterlockedIncrement(&refCount);
}

template<int i>
gMLock<i>::~gMLock() 
{ 
  if(::InterlockedDecrement(&refCount) == 0)
  {
    ::CloseHandle(hMutex);
    doLog("closing global mutex handle");
  }
}

template<int i>
void gMLock<i>::lock() 
{ 
  ::WaitForSingleObject(hMutex, INFINITE);
}

template<int i>
void gMLock<i>::unlock()
{
  ::ReleaseMutex(hMutex);
}

///////////////////////////////////////////////////////////////
// SRWLock - local lock class based on Win32 SRWLock

class SRWLock
{
public:
  SRWLock();
  ~SRWLock();
  void lockExclusive();
  void lockShared();
  void unlockExclusive();
  void unlockShared();
private:
  ::SRWLOCK srw;
};

inline SRWLock::SRWLock()
{
  ::InitializeSRWLock(&srw);
}

inline SRWLock::~SRWLock() 
{ 
  // no Destroy or Delete function
  // OS manages this - not clear how
}

inline void SRWLock::lockExclusive() 
{ 
  ::AcquireSRWLockExclusive(&srw);
}

inline void SRWLock::lockShared()
{
  ::AcquireSRWLockShared(&srw);
}

inline void SRWLock::unlockExclusive()
{
  ::ReleaseSRWLockExclusive(&srw);
}

inline void SRWLock::unlockShared()
{
  ::ReleaseSRWLockShared(&srw);
}

///////////////////////////////////////////////////////////////
// gSRWLock - global lock class based on Win32 SRWLock

template<int i>
class gSRWLock
{
public:
  gSRWLock();
  ~gSRWLock();
  void lockExclusive();
  void lockShared();
  void unlockExclusive();
  void unlockShared();
private:
  static ::SRWLOCK srw;
  static unsigned int refCount;
};

//----< statics are only initialized by first caller >---------

template<int i>
::SRWLOCK gSRWLock<i>::srw;
template<int i>
unsigned int gSRWLock<i>::refCount = 0;

template<int i>
gSRWLock<i>::gSRWLock()
{
  if(refCount==0)
  {
    doLog("Initializing SRWLock");
    ::InitializeSRWLock(&srw);
  }
  ::InterlockedIncrement(&refCount);
}

template<int i>
gSRWLock<i>::~gSRWLock() 
{ 
  if(::InterlockedDecrement(&refCount) == 0)
  {
    // do nothing
    doLog("SRWLock destructor called");
  }
}

template<int i>
void gSRWLock<i>::lockExclusive() 
{ 
  ::AquireSRWLockExclusive(&srw);
}

template<int i>
void gSRWLock<i>::lockShared() 
{ 
  ::AquireSRWLockShared(&srw);
}

template<int i>
void gSRWLock<i>::unlockExclusive()
{
  ::ReleaseSRWLockExclusive(&srw);
}

template<int i>
void gSRWLock<i>::unlockShared()
{
  ::ReleaseSRWLockShared(&srw);
}

///////////////////////////////////////////////////////////////////////////
// CSConditionVariable - local ConditionVariable based on CriticalSection
/*
 * ConditionVariables work in concert with a compatible lock to allow
 * a consumer to wait on an empty resource until a producer supplies
 * an instance for consumption.
 *
 * The ConditionVariable (CV) and lock must share an underlying synchronizing
 * construct, like a CriticalSection (CS).  The consumer locks the resource, then
 * passes its lock to the CV's sleep function, thus waiting inside the lock. 
 * The CV blocks the consumer but unlocks the lock so producers can get at the
 * resource.  When a producer has supplied a resource item it calls the CV's wake()
 * function to allow a waiting thread to consume.
 * 
 * This ConditionVariable is based on a CriticalSection, so you use a CriticalSection
 * based lock to pass to the sleep function.
 */

class CSConditionVariable
{
public:
  CSConditionVariable();
  ~CSConditionVariable();
  void sleep(CSLock& csl);
  void wake();
  void wakeAll();
private:
  ::CONDITION_VARIABLE cv;
};

inline CSConditionVariable::CSConditionVariable()
{
  ::InitializeConditionVariable(&cv);
}

inline CSConditionVariable::~CSConditionVariable()
{
  // nothing to do for ConditionVariable
}

inline void CSConditionVariable::sleep(CSLock& lock)
{
  ::SleepConditionVariableCS(&cv, (::CRITICAL_SECTION*)lock, INFINITE);
}

inline void CSConditionVariable::wake()
{
  ::WakeConditionVariable(&cv);
}

inline void CSConditionVariable::wakeAll()
{
  ::WakeAllConditionVariable(&cv);
}

///////////////////////////////////////////////////////////////////////////
// SRWConditionVariable - local ConditionVariable based on SRWLock

// todo: define SRWConditionVariable

/////////////////////////////////////////////////////////////////////
// syncOut class

class syncOut
{
public:
  syncOut(std::ostream& out);
  template <typename T>
  syncOut& operator<<(const T& t);
  syncOut& operator<<(syncOut& (*op)(syncOut&));
  template <typename T>
  void operator()(const T& t);
  void flush();

private:
  gCSLock<-1> _l;
  std::ostream& _out;
};
//----< constructor >------------------------------------------------

inline syncOut::syncOut(std::ostream& out) : _out(out) {}

//----< insertion >--------------------------------------------------

template <typename T>
syncOut& syncOut::operator<<(const T& t)
{
  _l.lock();
  _out << t;
  _out.flush();
  _l.unlock();
  return *this;
}
//----< insertion >--------------------------------------------------

inline syncOut& syncOut::operator<<(syncOut& (*op)(syncOut&))
{
  return (*op)(*this);
}
//----< functor >----------------------------------------------------

template <typename T>
void syncOut::operator()(const T& t)
{
  _l.lock();
  _out << t;
  _out.flush();
  _l.unlock();
}
//----< flush output stream >----------------------------------------

inline void syncOut::flush() { _out.flush(); }

//----< lock manipulator >-------------------------------------------

inline syncOut& locker(syncOut& so)
{
  gCSLock<-1>().lock();
  return so;
}

//----< unlock manipulator >-----------------------------------------

inline syncOut& unlocker(syncOut& so)
{
  gCSLock<-1>().unlock();
  return so;
}

extern syncOut sout;

/////////////////////////////////////////////////////////////////////
// debug Macro

inline void doLog(const char* pChar)
{
#ifdef DOLOG
  sout << locker << "\n  " << pChar << unlocker;
#endif
}


#endif
