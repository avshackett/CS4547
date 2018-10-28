//
//  Mom.hpp
//  Project9
//
//  Created by Alec Shackett on 12/1/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#pragma once
#ifndef Mom_hpp
#define Mom_hpp

#include <stdio.h>
#include "tools.hpp"
#include "Shared.hpp"
#include "Child.hpp"

extern void sigUsr1Handler(int sig);
extern bool startFlag;
extern bool quitFlag;
extern sigset_t emptySet;

class Mom{
private:
    enum Personality { lazy, overtired, prissy, greedy, impatient };
    char* childNames[4] = { (char*)"Elliot", (char*)"Jayce", (char*)"Sasha", (char*)"Krista" };
    Child children[4];
    pthread_t tid[4];
    Shared s;
    vector<Job> completed;
    time_t start;
    time_t current;
    int jCount = 0;
public:
    Mom() = default;
    void createJobList(Job *jobs);
    Job createJob();
    void checkForCompleted(Job *jobs);
    void run();
    int reward(Job *job);
};

#endif /* Mom_hpp */
