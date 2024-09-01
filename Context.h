//
// Created by user1 on 6/24/2024.
//

#ifndef _CONTEXT_H_
#define _CONTEXT_H_
#include "Barrier.h"
#include "JobHandler.h"
#include "MapReduceClient.h"
#include "MapReduceFramework.h"
#include <pthread.h>
#include <atomic>
#include <mutex>
#include <vector>  //std::vector
#include <utility> //std::pair
#include <semaphore.h>
#include <pthread.h>
#include <iostream>

class JobHandler;

class Context
{
 private:
  const MapReduceClient& client;


 public:
  JobHandler *jobHandler;
  const InputVec& inputVec;
  std::atomic<int> mapIndex;
  std::atomic<int> reduceIndex;
  std::atomic<int> numIntermediateElements; // Atomic counter for number of
  std::atomic<int> numOutputElements;
  Context(const MapReduceClient& client, int multiThreadLevel, const
  InputVec& inputVec, OutputVec& outputVec, JobHandler* jobHandler);
  Barrier *barrier;
  int num_of_threads;
  sem_t semaphore_shuffle;
  sem_t semaphore_reduce;
  std::vector<std::vector<IntermediatePair>> shuffleQueue; // Vector of
  // vectors as the queue . TODO in here or global?
  std::atomic<int> queueSize; // Atomic counter for queue size
  std::mutex outputMutex;
  std::mutex addIntermediateMutex;

  std::vector<IntermediateVec> threadsIntermediateVec;
  OutputVec& outputVec;



  ~Context();

  void addToIntermediate(IntermediatePair pair, int index);
  void addToOutPut(OutputPair pair);

  void useMap(const K1* key, const V1* value, void* context) const;
  void useReduce ( IntermediateVec* pairs, void* context) ;
};

#endif //_CONTEXT_H_
