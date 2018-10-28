//
//  Mom.cpp
//  Project9
//
//  Created by Alec Shackett on 12/1/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#include "Mom.hpp"

void Mom::run(){
    s = Shared();
    
    createJobList(s.jobs);
    
    sigemptyset(&emptySet);
    
    //Create the threads
    for(int x = 0; x < 4; ++x){
        children[x] = Child(childNames[x], &s);
        pthread_create( &tid[x], NULL, &Child::doChild, (void*)&children[x] );
    }
    sleep(1);
    time(&start);
    s.start = start;
    
    pthread_kill( tid[0], SIGUSR1);
    
    //Working loop
    for(;;){
        time(&current);
        int currentTime = difftime(current, start);
        pthread_mutex_lock( &s.turn_mutex );
        cout << "\nCurrent time is " << currentTime << endl;
        
        if(currentTime == 21){
            cout << "Work time is done!" << endl;
            quitFlag = true;
            startFlag = false;
            pthread_mutex_unlock( &s.turn_mutex );
            break;
        }
        pthread_mutex_unlock( &s.turn_mutex );
        
        pthread_mutex_lock( &s.turn_mutex );
        for(int x = 0; x < 10; ++x){
            if(s.jobs[x].checkDone()){
                cout << "Job " << s.jobs[x].getJID() << " completed by " << s.jobs[x].getCName() << endl;
                completed.push_back(s.jobs[x]);
                s.jobs[x] = createJob();
            }
        }
        pthread_mutex_unlock( &s.turn_mutex );
        sleep(1);
    }
    
    for(int x = 0; x < 4; ++x) pthread_kill(tid[x], SIGQUIT);

    pthread_join( tid[0], NULL);
    pthread_join( tid[1], NULL);
    pthread_join( tid[2], NULL);
    pthread_join( tid[3], NULL);
    
    pthread_mutex_lock( &s.turn_mutex );
    cout << "\nMom is totaling up the scores" << endl;
    int scores[4] = {0,0,0,0};
    //for(int x = 0; x < completed.size(); ++x) completed[x].print(cout);
    for(int x = 0; x < completed.size(); ++x){
        if((completed[x].checkDone()) && (strcmp(completed[x].getCName(), childNames[0]) == 0)){
            cout << "Elliot gets job " << completed[x].getJID() << endl;
            scores[0] += reward(&completed[x]);
        }
        else if((completed[x].checkDone()) && (strcmp(completed[x].getCName(), childNames[1]) == 0)){
            cout << "Jayce gets job " << completed[x].getJID() << endl;
            scores[1] += reward(&completed[x]);
        }
        else if((completed[x].checkDone()) && (strcmp(completed[x].getCName(), childNames[2]) == 0)){
            cout << "Sasha gets job " << completed[x].getJID() << endl;
            scores[2] += reward(&completed[x]);
        }
        else if((completed[x].checkDone()) && (strcmp(completed[x].getCName(), childNames[3]) == 0)){
            cout << "Krista gets job " << completed[x].getJID() << endl;
            scores[3] += reward(&completed[x]);
        }
    }
    
    int currentBest = 0;
    for(int x = 0; x < 4; ++x){
        if(scores[x] > scores[currentBest]) currentBest = x;
    }
    scores[currentBest] += 5;
    cout << childNames[currentBest] << " did best!" << endl;
    for(int x = 0; x < 4; ++x){
        cout << childNames[x] << ": $" << scores[x] << endl;
    }
    pthread_mutex_unlock( &s.turn_mutex );
}

int Mom::reward(Job *job){
    return job->getSlow() * (job->getDirty() + job->getHeavy());
}

void Mom::createJobList(Job *jobs){
    for(int x = 0; x < 10; ++x){
        jobs[x] = createJob();
    }
}

Job Mom::createJob(){
    return Job(++jCount, (rand() % 5) + 1, (rand() % 5) + 1, (rand() % 5) + 1);
}

void Mom::checkForCompleted(Job *jobs){
    for(int x = 0; x < 10; ++x){
        if(jobs[x].checkDone()){
            completed.push_back(jobs[x]);
            jobs[x] = createJob();
        }
    }
}
