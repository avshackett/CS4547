//
//  Child.hpp
//  Project9
//
//  Created by Alec Shackett on 12/1/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#pragma once
#ifndef Child_hpp
#define Child_hpp

#include <stdio.h>
#include "tools.hpp"
#include "Shared.hpp"

typedef struct  sigaction sigAct;
extern void sigUsr1Handler(int sig);
extern bool startFlag;
extern bool quitFlag;
extern sigset_t emptySet;

class Child{
private:
    enum Personality { lazy, overtired, prissy, greedy, impatient };
    char* moodList[5] = { (char*)"lazy", (char*)"overtired", (char*)"prissy", (char*)"greedy", (char*)"impatient" };
    vector<Job> completed;
    char* name;
    Shared *s;
    int mood;
    int currentMood();
    int selectJob(Job *jobs, int *length);
    int reward(Job *job){return job->getSlow() * (job->getDirty() + job->getHeavy());}
public:
    Child() = default;
    Child(char* name, Shared *s) : name(name), s(s) {}
    static void *doChild(void *self);
};

#endif /* Child_hpp */
