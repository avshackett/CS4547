//
//  Sweeper.hpp
//  Project4
//
//  Created by Alec Shackett on 9/22/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#ifndef Sweeper_hpp
#define Sweeper_hpp
#define DIVIDER "----------------------------------------------------------------"

#include "tools.hpp"
#include <getopt.h>
#include "Stats.hpp"
#include "Direntry.hpp"
#include "FileID.hpp"
#include <openssl/sha.h>

class Sweeper{
private:
    class Params{
    public:
        string state[2] = {"False", "True"};
        char* outFileName = nullptr;
        int minSize = 0;
        int dBLevel = 0;
        bool o = false;
        bool v = false;
        bool d = false;
        bool s = false;
        bool dB = false;
        char* path = NULL;
        const char *optString = "o:vds:0::";
        Params(int argc, char* const argv[]);
        void print(ostream* out);
        void usage();
    };
    char* path = NULL;
    Params p;
    vector<FileID> FIDvec;
    ofstream outStream;
    ostream* os;
    char* getSimple(char* p);
    char* concatPath(char* p, char* simple, bool file);
    vector<FileID>::iterator start, walker, end;
public:
    Sweeper(int argc, char* const argv[]) : p(argc, argv){
        if(p.o){
            outStream.open(p.outFileName);
            if(!outStream) fatal("Failed to open stream!");
            os = &outStream;
        }
        else os = &cout;
    }
    void run();
    void travel(char* pathName, char* simple, string tabs);
    void print(ostream& out) {for (unsigned x = 0; x < FIDvec.size(); ++x) FIDvec[x].print(out);}
    void controlBreakPoint(ostream& out);
    void slide();
    void checkDups();
    void printDups(ostream& out);
};
#endif /* Sweeper_hpp */
