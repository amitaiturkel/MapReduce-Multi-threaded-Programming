//
// Created by user1 on 6/24/2024.
//

#include "Context.h"
Context::Context(const MapReduceClient &client, int multiThreadLevel, const InputVec& inputVec, OutputVec& outputVec, JobHandler* jobHandler)
    : client(client),
      jobHandler(jobHandler),
      inputVec(inputVec),
      mapIndex(0),
      reduceIndex(0),
      numIntermediateElements(0),
      numOutputElements(0),
      barrier(new Barrier(multiThreadLevel)),
      num_of_threads(multiThreadLevel),
      queueSize(0),
      threadsIntermediateVec(std::vector<IntermediateVec>(multiThreadLevel)),
      outputVec(outputVec)
{
  sem_init(&semaphore_shuffle, 0, 0);
  sem_init(&semaphore_reduce, 0, 0);
}


Context::~Context()
{
  delete barrier;
  sem_destroy(&semaphore_shuffle);
  sem_destroy(&semaphore_reduce);
  delete jobHandler;
}

void Context::useMap (const K1 *key, const V1 *value, void *context) const
{
  client.map (key, value, context);
}
void Context::addToIntermediate (IntermediatePair pair, int index)
{
  // Lock output mutex before modifying outputVector
  threadsIntermediateVec[index].push_back(pair);
}
void Context::addToOutPut(OutputPair pair)
{
  outputMutex.lock();
  outputVec.push_back(pair);
  numOutputElements++;
  outputMutex.unlock();
}
void Context::useReduce (IntermediateVec* pairs,
                         void* context)
{


  client.reduce(pairs, context);
}
