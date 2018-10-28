//
//  main.cpp
//  Project8
//
//  Created by Alec Shackett on 11/15/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#include <iostream>
#include <vector>
#include "tools.hpp"
#include "Socket.hpp"
#include "Job.hpp"


#define BUFFSIZE 1024
#define PACKETSIZE sizeof(Instruction)

#define MAXCLIENTS 4
typedef struct pollfd    toPoll;

struct Instruction {
    int cID = 0;
    int jID = 0;
    int action = 0;
    Job *jobs = new Job[10];
    char message[16] = "Hello there.\0";
};

void printPacket(Instruction* i);
int reward(Job *job);
Job createJob(int *jCount);

int doWelcome(int welcomeSock, int* nClip, toPoll* worker);
int doService(toPoll* p, Job *jobs, vector<Job> *doneJobs, int *jCount, double *duration);
int getPort(int fd);
void printsockaddr_in(const char* who, sockaddr_in sock);
void serialize(Instruction* msgPacket, char *data);
void deserialize(char *data, Instruction* msgPacket);

enum Action { welcome, request, submit, choose, work, stop };

enum Personality { lazy, overtired, prissy, greedy, impatient };

int main(int argc, const char * argv[]) {
    banner();
    const char* proc = argv[0];
    if (argc<2) fatal("usage: %s port\n", proc);
    int port = strtol(argv[1], NULL, 10);
    
    char hostname[256];
    gethostname(hostname, 256);
    say("Mom's name is %s", hostname);
    int nCli = 0;
    int welcomeFd;
    
    toPoll ufds[MAXCLIENTS + 1];
    toPoll* const welcome = &ufds[0];
    toPoll* const worker = &ufds[1];
    int status;

    time_t start;
    time_t timer;
    
    double duration = 0;
    int jCount = 0;
    Job jobs[10];
    vector<Job> *doneJobs = new vector<Job>;
    srand(time(NULL));
    for(int x = 0; x < 10; ++x){
        jobs[x] = createJob(&jCount);
    }
    
    Socket server;
    server.listen(port);
    welcomeFd = server.sockfd();

    welcome->fd = welcomeFd;
    welcome->events = POLLIN;
    
    bool commence = false;
    for (;;) {
        if(commence){
            time(&timer);
            duration = difftime(timer,start);
            cout << endl << "Time: " << duration << endl;
        }
        status = poll(ufds, 1 + nCli, -1);
        if (status < 0)  fatalp("Error in poll().\t");
        if (status == 0) cout <<proc <<": poll timed out\n";

        if (welcome->revents != 0 )    {
            if (welcome->revents & POLLIN)    {
                if (nCli < MAXCLIENTS){
                    nCli += doWelcome(welcomeFd, &nCli, worker);
                    if(nCli == 4){
                        time(&start);
                        commence = true;
                    }
                }
            else fatalp("Error involving welcome mat: %d", welcome->revents);
            }
        }

        if(commence){
            int k;
            for (k = 0; k < nCli; k++) {
                if (worker[k].revents != 0) {
                    status = doService( &worker[k], jobs, doneJobs, &jCount, &duration );
                    if ( status == -1){
                        worker[k] = worker[--nCli];
                    }
                }
            }
        }
        if((duration >= 21) && (nCli == 0)){
            cout << "Workers left!" << endl;
            break;
        }

        welcome->events = (nCli < MAXCLIENTS) ? POLLIN : 0;
        
    }
    int jobLoop = sizeof(doneJobs);
    int score[4];
    for(int x = 0; x < jobLoop; ++x){
        score[doneJobs->at(x).getCID()] += reward(&doneJobs->at(x));
    }
    
    int currentBest = 0;
    for(int x = 0; x < 4; ++x){
        if(score[x] > score[currentBest]) currentBest = x;
    }
    score[currentBest] += 5;
    cout << "Child " << currentBest << " did best!" << endl;
    for(int x = 0; x < 4; ++x){
        cout << "Child " << x << ": $" << score[x] << endl;
    }
}

int doWelcome(int welcomeSock, int* nClip, toPoll* worker) {
    int nCli = *nClip;
    sockaddr_in newCaller;
    socklen_t sockLen = sizeof newCaller;
    
    int newfd = accept(welcomeSock, (sockaddr*) &newCaller, &sockLen);
    if (newfd < 0) {
        say("False alarm: packet was rejected.");
        return 0;
    }

    worker[nCli].fd = newfd;
    worker[nCli].events = POLLIN;
    worker[nCli].revents = 0;
    
    char data[BUFSIZ + 1];
    
    struct Instruction *i = new Instruction;
    i->cID = nCli;
    i->action = welcome;
    serialize(i, data);
    
    int bytes = write(newfd, data, sizeof(data));
    if (bytes < 1) say("Error while writing to socket");
    
    *nClip = nCli;
    cout << "Child " << nCli << " has arrived." << endl;
    return 1;
}

int doService(toPoll* p, Job* jobs, vector<Job> *doneJobs, int *jCount, double *duration) {
    char buf[BUFSIZ + 1];
    int retval = 0;

    if (p->revents & POLLIN) {
        int bytes = read(p->fd, buf, (sizeof buf ));
        if (bytes > 0) {
            buf[bytes] = '\0';
            struct Instruction *i = new Instruction;
            deserialize(buf, i);
            
            struct Instruction *outP = new Instruction;
            int action = i->action;

            if(*duration < 21){
                
                if(action == submit){
                    doneJobs->push_back(jobs[i->jID]);
                    jobs[i->jID] = createJob(jCount);
                    cout << "Child: " << i->cID << " submitted job " << i->jID << ".";
                    action = request;
                }
                if(action == request){
                    outP->action = choose;
                    cout << "Sending job list to child " << i->cID << endl;
                }
                if(action == choose){
                    if(jobs[i->jID].getCID() == -1){
                        jobs[i->jID].setChild(i->cID);
                        outP->action = work;
                        outP->jID = i->jID;
                        cout << "Job " << i->jID << " approved for child " << i->cID << "." << endl;
                    }
                    else{
                        outP->action = choose;
                        cout << "Pick a new job " << i->cID << "." << endl;
                    }
                }
            }
            else if(*duration >= 21){
                outP->action = stop;
            }
            outP->cID = i->cID;
            outP->jobs = jobs;
            char data[BUFSIZ + 1];
            serialize(outP, data);
            
            int bytes = write(p->fd, data, sizeof(data));
            if (bytes < 1) say("Error while writing to socket");
        }
        else if (bytes == 0) {
            printf("\nclosing socket on port %d\n", getPort(p->fd));
            close(p->fd);
            retval = -1;
        } else if (errno == ECONNRESET) {
            sayp("socket %d disappeared", getPort(p->fd));
            close(p->fd);
            retval = -1;
        } else {
            fatalp("Error %d from read, port %d", bytes, getPort(p->fd));
        }
    }
    else if (p->revents & POLLHUP) {
        say("Removing dead socket %d\n", getPort(p->fd));
        close(p->fd);
        retval = -1;
    }
    return retval;
}

int getPort(int fd){
    sockaddr_in client;
    socklen_t sockLen = sizeof client;
    int status = getpeername(fd, (sockaddr*) &client, &sockLen);
    if (status<0) fatalp("Can't get port# of socket (%d)", fd);
    return ntohs(client.sin_port);
}

Job createJob(int *jCount){
    int count = *jCount;
    ++count;
    *jCount = count;
    return Job(count, (rand() % 5) + 1, (rand() % 5) + 1, (rand() % 5) + 1);
}

int reward(Job *job){
    return job->getSlow() * (job->getDirty() + job->getHeavy());
}

void serialize(Instruction *instrPacket, char *data){
    int *q = (int*)data;
    *q = instrPacket->cID; q++;
    *q = instrPacket->action; q++;
    *q = instrPacket->jID; q++;
    
    for(int x = 0; x < 10; ++x){
        *q = instrPacket->jobs[x].getJID(); q++;
        *q = instrPacket->jobs[x].getSlow(); q++;
        *q = instrPacket->jobs[x].getDirty(); q++;
        *q = instrPacket->jobs[x].getHeavy(); q++;
        *q = instrPacket->jobs[x].getCID(); q++;
    }
}

void deserialize(char *data, Instruction* instrPacket){
    int *q = (int*)data;
    instrPacket->cID = *q; q++;
    instrPacket->action = *q; q++;
    instrPacket->jID = *q; q++;
    
    for(int x = 0; x < 10; ++x){
        int jID = *q; q++;
        int slow = *q; q++;
        int dirty = *q; q++;
        int heavy = *q; q++;
        int cID = *q; q++;
        instrPacket->jobs[x] = Job(jID, slow, dirty, heavy);
        instrPacket->jobs[x].setChild(cID);
    }
}

void printPacket(Instruction* i){
    cout << i->action << " " << i->cID << " " << i->jID << endl;
    for(int x = 0; x < 10; ++x) i->jobs[x].print(cout);
}
