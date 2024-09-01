#include "MapReduceFramework.h"
#include "MapReduceClient.h"
#include "WarpContext.h"
#include "Context.h"
#include <stdio.h>
#include <cstring>  // for std::strerror
#include <mutex>
#include <semaphore.h>
#include <stdlib.h>
#include <algorithm>
#define ERROR_MSG "system error"

std::vector<pthread_t> threads;
std::mutex waitMutex; // Mutex to synchronize access to alreadyWaited



void emit2 (K2* key, V2* value, void* context)
{
  auto warpContext = static_cast<WarpContext*>(context);
  auto context1 = warpContext->context;
  if( key&& value)
  {
    context1->addToIntermediate (std::make_pair (key, value), warpContext->getThreadIndex ());
  }
  else{
    std::printf ("or key or value is null");
  }
}
void emit3 (K3* key, V3* value, void* context){
  auto warpContext = static_cast<WarpContext*>(context);
  auto context1 = warpContext->context;
  context1->addToOutPut(std::make_pair(key, value));
}

/**
 * Active the map phase for all the threads
 */
void activeMapPhase(WarpContext* warpContext)
{

  auto context = warpContext->context;


  context->jobHandler->setJobHandlerTotalElements(context->inputVec.size());
  context->jobHandler->setJobHandlerState (MAP_STAGE);
  int vectorSize = context->inputVec.size();
  while (true) {
    // Atomically fetch and increment the index
    int index = context->mapIndex++;

    // Break if the index is out of range
    if (index >= vectorSize) {
      break;
    }
    context->useMap(context->inputVec[index].first,context->inputVec[index].second, warpContext);
    context->jobHandler->addJobHandlerElements();
  }
}

/**
 * Active the sort phase for all the threads
 */void activeSortPhase(WarpContext* warpContext)
{
  auto context = warpContext->context;
  // Sort the vector of IntermediatePair
  std::sort(context->threadsIntermediateVec[warpContext->getThreadIndex()].begin(),
            context->threadsIntermediateVec[warpContext->getThreadIndex()].end(),
            [](IntermediatePair& a, IntermediatePair& b)
            {
                if (a.first == nullptr && b.first == nullptr) {
                  return false; // Both are null, they are equal
                }
                if (a.first == nullptr) {
                  return true; // a is null, b is not
                }
                if (b.first == nullptr) {
                  return false; // b is null, a is not
                }
                // Both are non-null, compare the values
                return *(a.first) < *(b.first);
            });
  }

bool allThreadsIntermediateVecEmpty(WarpContext* warpContext) {
  auto context = warpContext->context;
  for (const auto& vec : context->threadsIntermediateVec) {
    if (!vec.empty()) {
      return false;
    }
  }
  return true;
}
K2* findMaxKey(Context* context) {
  K2* maxKey = nullptr;
  int numThreads = context->threadsIntermediateVec.size();

  for (int i = 0; i < numThreads; ++i) {
    if (!context->threadsIntermediateVec[i].empty()) {
      IntermediatePair backPair = context->threadsIntermediateVec[i].back();
      if (maxKey == nullptr || *maxKey < *(backPair.first)) {
        maxKey = backPair.first;
      }
    }
  }
  return maxKey;
}
std::vector<IntermediatePair> getAllSameKeys(WarpContext* warpContext, K2 *current_key) {
  int numThreads = warpContext->context->num_of_threads;
  Context* context = warpContext->context;
  std::vector<IntermediatePair> currentKeyGroup;

  for (int i = 0; i < numThreads; ++i) {
    auto& threadVec = context->threadsIntermediateVec[i];

    // Check if the thread's vector is not empty and the back pair's key is equal to current_key
    while (!threadVec.empty() && !(*threadVec.back().first < *current_key)) {
      currentKeyGroup.push_back(threadVec.back());
      threadVec.pop_back();
    }
  }

  return currentKeyGroup;
}


void activeShufflePhase(WarpContext* warpContext){


  int numThreads = warpContext->context->num_of_threads;
  Context* context = warpContext->context;
  // update job handler
  int64_t shuffle_num = (static_cast<int64_t>(context->threadsIntermediateVec.size()) << 31) |
                        (static_cast<int64_t>(SHUFFLE_STAGE) << 62);

  context->jobHandler->setJobHandlerAll (shuffle_num);
//  context->jobHandler->setJobHandlerElements(0);
//  context->jobHandler->setJobHandlerTotalElements
//      (context->threadsIntermediateVec.size());
//  context->jobHandler->setJobHandlerState(SHUFFLE_STAGE);



  while (!allThreadsIntermediateVecEmpty(warpContext))
  {
    K2 *current_key = findMaxKey (context);
    std::vector<IntermediatePair> currentKeyGroup = getAllSameKeys
        (warpContext,current_key) ;
    context->shuffleQueue.push_back (currentKeyGroup);
    context->queueSize++;
    // elhanan did quesize++ for each pair
    context->jobHandler->setJobHandlerElements(context->queueSize);
  }
  // empty JobHandler for next stage
  int64_t reduce_num = (static_cast<int64_t>(context->queueSize) << 31) |
                       (static_cast<int64_t>(REDUCE_STAGE) << 62);

  context->jobHandler->setJobHandlerAll (reduce_num);
}


void activeReducePhase(WarpContext* warp_context)
{

  Context *context = warp_context->context;
  int vectorSize = context->shuffleQueue.size();

  while (true) {
    // Atomically fetch and increment the index
    int index = context->reduceIndex++;

    // Break if the index is out of range
    if (index >= vectorSize) {
      break;
    }

    context->useReduce (&context->shuffleQueue[index], warp_context);
    context->jobHandler->addJobHandlerElements();
  }
}
  void *f(void * arg)
{
  auto warpContext =  static_cast<WarpContext*> (arg);
  auto context = warpContext->context;
  activeMapPhase(warpContext);
  activeSortPhase(warpContext);
  warpContext->context->barrier->barrier(); // active barrier,all threads
  // sorted
  if(warpContext->getThreadIndex() == 0)
  {
    activeShufflePhase (warpContext);

    for (int i = 1; i < warpContext->context->num_of_threads; ++i)
    {
      if(sem_post(&context->semaphore_shuffle) != 0){
        perror (ERROR_MSG);
        exit(1);
      }

    }
  }
  else{
    if(sem_wait(&context->semaphore_shuffle) != 0) {
      perror (ERROR_MSG);
      exit(1);
    }
  }

  activeReducePhase(warpContext);
  return (void*)NULL;

}



JobHandle startMapReduceJob(const MapReduceClient& client,
                            const InputVec& inputVec, OutputVec& outputVec,
                            int multiThreadLevel)
{
  JobHandler *jobHandler = new JobHandler();
  jobHandler->setJobHandlerAll (0);
  Context *context = new Context(client, multiThreadLevel, inputVec,
                                 outputVec,jobHandler);
  for (int i = multiThreadLevel - 1; i >= 0; --i) {
      jobHandler->warpVector.push_back(new WarpContext(i, context));
  }


  // create threads
  jobHandler->threads = std::vector<pthread_t>(multiThreadLevel);
  for (int i = 0; i < multiThreadLevel; ++i)
  {
    if(pthread_create(&(jobHandler->threads[i]), nullptr, f, static_cast<void *>
    (jobHandler->warpVector[i]))
       != 0)
    {
      perror(ERROR_MSG);
      exit(1);
    }
  }

  return context;

}void waitForJob(JobHandle job)
{
  Context* context = static_cast<Context*>(job);
  auto jobHandler = context->jobHandler;
  std::unique_lock<std::mutex> lock (waitMutex); // Lock the mutex for synchronized access

  if (jobHandler->alreadyWaited)
  {
    return;
  }
  jobHandler->alreadyWaited = true;
  lock.unlock ();
  for (pthread_t thread: jobHandler->threads)
  {
    int rc = pthread_join (thread, nullptr);
    if (rc != 0)
    {
      fprintf (stderr, "Error: pthread_join() failed with code %d\n", rc);
      if (rc == ESRCH)
      {
        fprintf (stderr, "Thread does not exist or already joined (ESRCH)\n");
      }
      else if (rc == EINVAL)
      {
        fprintf (stderr, "Thread is not joinable (EINVAL)\n");
      }
      else if (rc == EDEADLK)
      {
        fprintf (stderr, "Deadlock detected (EDEADLK)\n");
      }
      else
      {
        perror (ERROR_MSG); // This may not give useful output, as errno is not set
      }
      exit (1);
    }
  }
}



void getJobState(JobHandle job, JobState* state)
{
  Context* context = static_cast<Context*>(job);
  int64_t jobHandlerValues = context->jobHandler->getJobStateValues();
  state->stage = (stage_t)((jobHandlerValues >> 62) & 0x3);
  state->percentage = std::min(static_cast<float>((jobHandlerValues & 0x3FFFFFFF)) /
                               static_cast<float>(((jobHandlerValues >> 31) & 0x3FFFFFFF)) * 100.0f,
                               100.0f);

}


void closeJobHandle(JobHandle job)
{
  Context* context = static_cast<Context*>(job);
  waitForJob(job);
  delete context;
}


