//
// Created by user1 on 6/27/2024.
//

#ifndef _JOBHANDLER_H_
#define _JOBHANDLER_H_
#include "Context.h"
#include <vector>
#include <atomic>

class WarpContext;

class Context;

class JobHandler
{
 private:

 public:
  std::atomic<int64_t> jobStateValues; // 2 bits - stage, 31 bits -
  // total elements, 31 bits - elements
  bool alreadyWaited;
  std::vector<WarpContext*> warpVector;
  std::vector<pthread_t> threads;

  JobHandler();
  ~JobHandler();
  void setJobHandlerState(int64_t stage);
  void setJobHandlerElements(int64_t elements);
  void setJobHandlerTotalElements(int64_t totalElements);
  void addJobHandlerElements();
  int64_t getJobStateValues();
  void addContext(Context *context);

  void
  setJobHandlerAll (int64_t stage);
};

#endif //_JOBHANDLER_H_