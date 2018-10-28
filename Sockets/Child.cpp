//
//  Child.cpp
//  Project8
//
//  Created by Alec Shackett on 11/15/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#include <iostream>
#include "tools.hpp"
#include "Socket.hpp"
#include "Job.hpp"

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
void serialize(Instruction *instrPacket, char *data);
void deserialize(char *data, Instruction* instrPacket);
int getPort(int fd);
void printsockaddr_in(const char* who, sockaddr_in sock);
int selectJob(Job *jobs, int *personality);

enum Action { welcome, request, submit, choose, work, stop };

enum Personality { lazy, overtired, prissy, greedy, impatient };

int main(int argc, char * argv[]) {
    if (argc<3) fatal("usage: %s host port\n", argv[0]);
    char* proc = argv[0];
    char* servername = argv[1];
    int   port = strtol(argv[2],NULL,10);
    
    int cID;
    int jobsDone = 0;
    srand(time(NULL));
    int personality = (rand() % 5);
    
    switch (personality) {
        case lazy:
            cout << "I am lazy today!" << endl;
            break;
        case overtired:
            cout << "I am overtired today!" << endl;
            break;
        case prissy:
            cout << "I am prissy today!" << endl;
            break;
        case greedy:
            cout << "I am greedy today!" << endl;
            break;
        case impatient:
            cout << "I am impatient today!" << endl;
            break;
        default:
            break;
    }

    char buf[BUFSIZ+1];
    int nBytes;

    Socket client;
    client.connect(servername, port);
    //client.printPeer(cout);

    nBytes = read( client.sockfd(), buf, sizeof buf );
    if ( nBytes > 0 )  cout << buf;
    else fatalp("%s: Error while reading from socket.", proc );
    
    struct Instruction *i = new Instruction;
    deserialize(buf, i);
    
    cID = i->cID;
    
    cout << "Arrived at mom with id " << cID << "." << endl;
    cout << "Sending out job request." << endl;
    struct Instruction *outP = new Instruction;
    
    outP->cID = cID;
    outP->action = request;
    
    char *data = new char[BUFSIZ + 1];
    serialize(outP, data);
    
    int bytes = write( client.sockfd(), data, sizeof(data));
    if (bytes < 1) say("Error while writing to socket");
    cout << "Request sent." << endl;
    
    for(;;){
        cout << endl;
        int nBytes = read( client.sockfd(), buf, (sizeof buf) );
        if(nBytes > 0){
            struct Instruction *i = new Instruction;
            deserialize(buf, i);
            int action = i->action;

            struct Instruction *outP = new Instruction;
            
            if(action == choose){
                outP->jID = selectJob(i->jobs, &personality);
                outP->action = choose;
                cout << "Choosing job: " << outP->jID << endl;
            }
            if(action == work){
                cout << "Working for " << i->jobs[i->jID].getSlow() << " seconds" << endl;
                usleep(i->jobs[i->jID].getSlow() * 1000000);
                
                outP->jID = i->jID;
                outP->action = submit;
                cout << "Done!" << endl;
                ++jobsDone;
            }
            if(action == stop){
                cout << "Work time is done!" << endl;
                break;
            }
            outP->cID = cID;
            outP->jobs = i->jobs;
            
            char data[BUFSIZ + 1];
            serialize(outP, data);
            
            int bytes = write( client.sockfd(), data, sizeof(data));
            if (bytes < 1) say("Error while writing to socket");
        }
        
    }
    cout << "Jobs done " << jobsDone << endl;
    exit(0);
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

int getPort(int fd){
    sockaddr_in client;
    socklen_t sockLen = sizeof client;
    int status = getpeername(fd, (sockaddr*) &client, &sockLen);
    if (status<0) fatalp("Can't get port# of socket (%d)", fd);
    return ntohs(client.sin_port);
}

void printsockaddr_in(const char* who, sockaddr_in sock) {
    printf("%s socket is  {\n\t"
           "sin_family        = %d\n\t"
           "sin_addr.s_addr   = %s\n\t"
           "sin_port          = %d\n\t"
           "}\n", who, sock.sin_family, inet_ntoa(sock.sin_addr),
           ntohs(sock.sin_port));
}

int selectJob(Job *jobs, int *personality){
    int cPersonality = *personality;
    int currentBest = 0;
    if(cPersonality == impatient){
        for(;;){
            currentBest = rand() % 10;
            if(jobs[currentBest].getCID() == -1) break;
        }
    }
    else if(cPersonality == greedy){
        for(int x = 0; x < sizeof(jobs); ++x){
            if((reward(&jobs[x]) > reward(&jobs[currentBest])) && (jobs[x].getCID() == -1)) currentBest = x;
        }
    }
    else{
        bool done = false;
        for(short int val = 1; val < 6; ++val){
            for(int x = 0; x < sizeof(jobs); ++x){
                switch (cPersonality) {
                    case lazy:
                        if((jobs[x].getHeavy() == val) && (jobs[x].getCID() == -1)){
                            currentBest = x;
                            done = true;
                        }
                        break;
                    case overtired:
                        if((jobs[x].getSlow() == val) && (jobs[x].getCID() == -1)){
                            currentBest = x;
                            done = true;
                        }
                        break;
                    case prissy:
                        if((jobs[x].getDirty() == val) && (jobs[x].getCID() == -1)){
                            currentBest = x;
                            done = true;
                        }
                        break;
                    default:
                        break;
                }
            }
            if(done) break;
        }
        
    }
    return currentBest;
}

int reward(Job *job){
    return job->getSlow() * (job->getDirty() + job->getHeavy());
}

void printPacket(Instruction* i){
    cout << i->action << " " << i->cID << " " << i->jID << endl;
    for(int x = 0; x < 10; ++x) i->jobs[x].print(cout);
}
