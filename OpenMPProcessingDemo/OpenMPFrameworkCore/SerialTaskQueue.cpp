// -*- C++ -*-
//
// Package:     Concurrency
// Class  :     SerialTaskQueue
// 
// Implementation:
//     [Notes on implementation]
//
// Original Author:  Chris Jones
//         Created:  Thu Feb 21 11:31:52 CST 2013
// $Id$
//

// system include files

// user include files
#include "SerialTaskQueue.h"

#include "Likely.h"

using namespace demo;

//
// member functions
//
bool
SerialTaskQueue::resume() {
  if(0==--m_pauseCount) {
    auto t = pickNextTask();
    if(nullptr != t) {
      demo::TaskBase::spawn(t);
    }
    return true;
  }
  return false;
}

void
SerialTaskQueue::pushTask(TaskBase* iTask) {
  auto t = pushAndGetNextTask(iTask);
  if(nullptr!=t) {
    demo::TaskBase::spawn(t);      
  }
}

demo::TaskBase* 
SerialTaskQueue::pushAndGetNextTask(TaskBase* iTask) {
  demo::TaskBase* returnValue{nullptr};
  if likely(nullptr!=iTask) {
    m_tasks.push(iTask);
    returnValue = pickNextTask();
  }
  return returnValue;
}


demo::TaskBase*
SerialTaskQueue::finishedTask() {
  m_taskChosen.clear();
  return pickNextTask();
}

SerialTaskQueue::TaskBase*
SerialTaskQueue::pickNextTask() {
  
  if likely(0 == m_pauseCount and not m_taskChosen.test_and_set()) {
    TaskBase* t=0;
    if likely(m_tasks.try_pop(t)) {
      return t;
    }
    //no task was actually pulled
    m_taskChosen.clear();
    
    //was a new entry added after we called 'try_pop' but before we did the clear?
    if(not m_tasks.empty() and not m_taskChosen.test_and_set()) {
      TaskBase* t=0;
      if(m_tasks.try_pop(t)) {
        return t;
      }
      //no task was still pulled since a different thread beat us to it
      m_taskChosen.clear();
      
    }
  }
  return 0;
}

/*
void SerialTaskQueue::pushAndWait(tbb::empty_task* iWait, TaskBase* iTask) {
   auto nextTask = pushAndGetNextTask(iTask);
   if likely(nullptr != nextTask) {
     if likely(nextTask == iTask) {
        //spawn and wait for all requires the task to have its parent set
        iWait->spawn_and_wait_for_all(*nextTask);
     } else {
        tbb::task::spawn(*nextTask);
        iWait->wait_for_all();
     }
   } else {
     //a task must already be running in this queue
     iWait->wait_for_all();              
   }
   tbb::task::destroy(*iWait);
}
*/

//
// const member functions
//

//
// static member functions
//
