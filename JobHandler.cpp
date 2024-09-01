#include "JobHandler.h"

void JobHandler::setJobHandlerState(int64_t stage)
{
    jobStateValues = (jobStateValues & 0x3FFFFFFFFFFFFFFF) | (stage << 62);
}

void JobHandler::setJobHandlerElements(int64_t elements)
{
    jobStateValues = (jobStateValues & 0xFFFFFFFF80000000) | elements;
}
void JobHandler::setJobHandlerTotalElements(int64_t totalElements)
{
  jobStateValues = (jobStateValues & 0xC00000007FFFFFFF) | (totalElements
      << 31);
}
void JobHandler::setJobHandlerAll(int64_t num)
{
  jobStateValues = num;
}
void JobHandler::addJobHandlerElements()
{
  jobStateValues++;
}

int64_t JobHandler::getJobStateValues()
{
    return jobStateValues;
}

JobHandler::JobHandler ()
{
    alreadyWaited = false;
    jobStateValues = 0;
}


JobHandler::~JobHandler()
{
    // If JobHandler owns the context, it should delete it to prevent memory leaks
    for (auto WarpContext: warpVector){
      delete WarpContext;
    }

}