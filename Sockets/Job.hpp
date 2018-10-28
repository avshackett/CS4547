//
//  Job.hpp
//  Project8
//
//  Created by Alec Shackett on 11/17/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#ifndef Job_hpp
#define Job_hpp

#include <stdio.h>
#include <iostream>

using namespace std;

class Job{
private:
    short int slow = 0, dirty = 0, heavy = 0, jID = 0, cID = -1;
public:
    Job() = default;
    Job(int jID, short int slow, short int dirty, short int heavy) : jID(jID), slow(slow), dirty(dirty), heavy(heavy){}
    void print(ostream& out){out << "Job " << jID << " Child " << cID << " " << slow << " " << dirty << " " << heavy << endl;}
    int getJID(){return jID;}
    int getCID(){return cID;}
    short int getSlow(){return slow;}
    short int getDirty(){return dirty;}
    short int getHeavy(){return heavy;}
    void setChild(short int cID){this->cID = cID;}
};
#endif /* Job_hpp */
