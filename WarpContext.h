//
// Created by user1 on 6/25/2024.
//

#ifndef _WARPCONTEXT_H_
#define _WARPCONTEXT_H_
#include "Context.h"

class WarpContext
{
 private:

  int threadIndex;
 public:
  Context* context;
  WarpContext(int threadIndex, Context *context);
  int getThreadIndex() const{return threadIndex;};
};

#endif //_WARPCONTEXT_H_
