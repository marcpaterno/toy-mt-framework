/*
 *  Queues.cpp
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */

#include "Queues.h"
#include "ConcurrentTaskQueue.h"
#include "SerialTaskQueue.h"
namespace demo {
   boost::shared_ptr<ConcurrentTaskQueue> s_thread_safe_queue(new ConcurrentTaskQueue{});
   boost::shared_ptr<SerialTaskQueue> s_non_thread_safe_queue(new SerialTaskQueue{});
}