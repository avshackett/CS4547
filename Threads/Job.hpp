//
//  Job.hpp
//  Project9
//
//  Created by Alec Shackett on 12/1/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#ifndef Job_hpp
#define Job_hpp

#include <stdio.h>
#include <iostream>

using namespace std;

class Job{
private:
    int jID = 0;
    short int slow = 0, dirty = 0, heavy = 0;
    char* cName = (char*)"";
    bool done = false;
public:
    Job() = default;
    Job(int jID, short int slow, short int dirty, short int heavy) : jID(jID), slow(slow), dirty(dirty), heavy(heavy){}
    void print(ostream& out){out << "Job " << jID << " Child " << cName << " " << slow << " " << dirty << " " << heavy << endl;}
    int getJID(){return jID;}
    bool checkDone(){return this->done;}
    void setDone(){this->done = true;}
    char* getCName(){return cName;}
    short int getSlow(){return slow;}
    short int getDirty(){return dirty;}
    short int getHeavy(){return heavy;}
    void setChild(char* cName){this->cName = cName;}
    bool unassigned(){if(strcmp(cName, "") == 0) return true; else return false;}
    time_t handInTime;
};
#endif /* Job_hpp */

