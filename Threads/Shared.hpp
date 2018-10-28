//
//  Shared.hpp
//  Project9
//
//  Created by Alec Shackett on 12/1/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#pragma once
#ifndef Shared_hpp
#define Shared_hpp

#include <stdio.h>
#include "tools.hpp"
#include "Job.hpp"

extern bool startFlag;

class Shared{
public:
    Shared() = default;
    pthread_mutex_t turn_mutex  = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t turn = PTHREAD_COND_INITIALIZER;
    time_t start;
    Job *jobs = new Job[10];
    Job* getJobList(){return jobs;}
    void markDone(int pos){jobs[pos].setDone();}
};
#endif /* Shared_hpp */
