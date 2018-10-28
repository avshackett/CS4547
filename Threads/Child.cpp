//
//  Child.cpp
//  Project9
//
//  Created by Alec Shackett on 12/1/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#include "Child.hpp"



void *Child::doChild(void *self){
    Child *c = (Child*)self;
    c->mood = c->currentMood();
    
    int rc;
    int pos;
    int length;
    sigset_t pending;
    sigset_t set;
    //sigset_t emptySet;
    //sigemptyset(&emptySet);
    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGUSR1);
    rc = pthread_sigmask(SIG_BLOCK, &set, NULL);
    
    //Setup stuff
    sigAct workAct;
    workAct.sa_handler = sigUsr1Handler;     // handler to call
    workAct.sa_mask = emptySet;            // signals to disable in handler
    workAct.sa_flags = 0;                // 0 means the default settings
    sigaction(SIGUSR1,  &workAct, NULL);
    
    pthread_mutex_lock( &c->s->turn_mutex );
    cout << c->name << " is " << c->moodList[c->mood] << " and waiting to start!" << endl;
    pthread_mutex_unlock( &c->s->turn_mutex );
    
    
    //The problem is here somewhere
    while(!startFlag){
        sigpending(&pending);
        if (sigismember(&pending, SIGUSR1)) {
            sigUsr1Handler(SIGUSR1);
            break;
        }
    }
    
    while( startFlag ) {
        // Entering critical section. Block if queue is in use or empty.
        if (!quitFlag && startFlag) {
            pthread_mutex_lock( &c->s->turn_mutex );
            while(!quitFlag) pthread_cond_wait(&c->s->turn, &c->s->turn_mutex);
            pos = c->selectJob(c->s->jobs, &length);
            cout << "My name is " << c->name << " and I am doing job " << c->s->jobs[pos].getJID() << " for " << c->s->jobs[pos].getSlow() << " minutes" << endl;
            pthread_cond_signal( &c->s->turn );
            pthread_mutex_unlock( &c->s->turn_mutex );
        // Leaving critical section
        }
        
        sigpending(&pending);
        if(sigismember(&pending, SIGQUIT)) {
            pthread_mutex_lock( &c->s->turn_mutex );
            cout << c->name << " is going home" << endl;
            pthread_mutex_unlock( &c->s->turn_mutex );
            break;
        }
        
        
        if(!quitFlag){
            usleep(length * 1000000);
            pthread_mutex_lock( &c->s->turn_mutex );
            while(!quitFlag) pthread_cond_wait(&c->s->turn, &c->s->turn_mutex);
            c->s->markDone(pos);
            time( &c->s->jobs[pos].handInTime );
            c->completed.push_back(c->s->getJobList()[pos]);
            //cout << c->completed.size() << " jobs done by " << c->name << endl;
            pthread_cond_signal( &c->s->turn );
            pthread_mutex_unlock( &c->s->turn_mutex );
        }
    }

    pthread_mutex_lock( &c->s->turn_mutex );
    cout << "\nDone working! Here are the jobs I, " << c->name << ", have done:" << endl;
    int score = 0;
    for(int x = 0; x < c->completed.size(); ++x){
        if(difftime(c->completed[x].handInTime, c->s->start) < 21){
            c->completed.at(x).print(cout);
            score += c->reward(&c->completed.at(x));
        }
    }
    cout << "My name is " << c->name << " and my earnings are $" << score << endl << endl;
    pthread_mutex_unlock( &c->s->turn_mutex );
    return (0);
}

int Child::currentMood(){
    return rand() % 5;
}

int Child::selectJob(Job *jobs, int *length){
    int currentBest = 0;
    if(mood == impatient){
        for(;;){
            currentBest = rand() % 10;
            if(jobs[currentBest].unassigned()){
                jobs[currentBest].setChild(name);
                break;
            }
        }
    }
    else if(mood == greedy){
        for(int x = 0; x < sizeof(jobs); ++x){
            if((reward(&jobs[x]) > reward(&jobs[currentBest])) && jobs[x].unassigned()) currentBest = x;
        }
        jobs[currentBest].setChild(name);
    }
    else if(mood == lazy){
        for(short int val = 1; val < 6; ++val){
            for(int x = 0; x < sizeof(jobs); ++x){
                if((jobs[x].getHeavy() == val) && jobs[x].unassigned()){
                    currentBest = x;
                    jobs[currentBest].setChild(name);
                    val = 7;
                    break;
                }
            }
        }
    }
    else if(mood == overtired){
        for(short int val = 1; val < 6; ++val){
            for(int x = 0; x < sizeof(jobs); ++x){
                if((jobs[x].getSlow() == val) && jobs[x].unassigned()){
                    currentBest = x;
                    jobs[currentBest].setChild(name);
                    val = 7;
                    break;
                }
            }
        }
    }
    else if(mood == prissy){
        for(short int val = 1; val < 6; ++val){
            for(int x = 0; x < sizeof(jobs); ++x){
                if((jobs[x].getDirty() == val) && jobs[x].unassigned()){
                    currentBest = x;
                    jobs[currentBest].setChild(name);
                    val = 7;
                    break;
                }
            }
        }
    }
    *length = jobs[currentBest].getSlow();
    return currentBest;
}
