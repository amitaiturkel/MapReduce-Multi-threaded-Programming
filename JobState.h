//
// Created by home on 6/27/2024.
//

#ifndef _JOBSTATE_H_
#define _JOBSTATE_H_
typedef void* JobHandle;

enum stage_t {UNDEFINED_STAGE=0, MAP_STAGE=1, SHUFFLE_STAGE=2, REDUCE_STAGE=3};

typedef struct {
    stage_t stage;
    float percentage;
} JobState;

#endif //_JOBSTATE_H_
