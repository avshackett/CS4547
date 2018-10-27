//
//  Sweeper.cpp
//  Project4
//
//  Created by Alec Shackett on 9/22/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#include "Sweeper.hpp"

Sweeper::Params::Params(int argc, char* const argv[]){
    struct option longOpts[] = {
        { "verbose",    no_argument,       NULL,  'v' },
        { "output",     required_argument, NULL,  'o' },
        { "delete",     no_argument,       NULL,  'd' },
        { "min",        required_argument, NULL,  's' },
        { "debug",      optional_argument, NULL,  '0' },
        { NULL,         0,                 NULL,   0  }
    };
    
    int ch, code, required;
    for(;;){
        ch = getopt_long(argc, argv, optString, longOpts, &code);
        if(ch == -1) break;
        switch(ch){
            case 'v':
                v = true;
                break;
            case 'd':
                d = true;
                break;
            case 's':
                s = true;
                minSize = atoi(optarg);
                break;
            case 'o':
                o = true;
                outFileName = optarg;
                break;
            case '0':
                dB = true;
                if(optarg != NULL) {
                    char ch = optarg[1];
                    dBLevel = atoi(&ch);
                }
                if(dBLevel < 0 || dBLevel > 2) usage();
                break;
            default:
                usage();
                break;
        }
    }
    required = optind;
    if(required != (argc - 1)) usage();
    else path = argv[required];
}

void Sweeper::Params::print(ostream* out){
    *out << "Output file: " << (o ? outFileName : "No output file") << endl;
    *out << "Verbosity: " << state[v] << endl;
    *out << "Delete duplicates: " << state[d] << endl;
    *out << "Minimum file size: " << minSize << "K" << endl;
    *out << "Debug: " << state[dB];     (dB ? printf(", Level = %d\n", dBLevel) : printf("\n") );
    *out << "Directory: " << path << endl;
    cout << (o ? "Outputted to file " + (string)outFileName : "Outputted to terminal") << endl;
}

void Sweeper::Params::usage(){
    cerr << "Usage: sweep [-vd] [-0=(0-2)] [-s minSize] [-o filename] [--verbose] path" << endl;
    exit(1);
}

void Sweeper::run(){
    p.print(os);
    
    if(p.path[0] == '.'){
        string temp = p.path;
        temp.erase(0, 2);
        char* cwd = new char[PATH_MAX];
        getcwd(cwd, PATH_MAX);
        string sPath = (string)cwd + "/" + temp + "/";
        path = new char[PATH_MAX];
        strcpy(path, sPath.c_str());
    }
    else path = strcat(p.path, "/");

    chdir(path);
    
    if(p.v){
        *os << "VERBOSE OUTPUT" << endl;
        *os << DIVIDER << endl;
    }
    travel(path, path, "");
    if(p.v) *os << DIVIDER << endl;
    
    sort(FIDvec.begin(), FIDvec.end(), FileID::less);
    
    *os << DIVIDER << endl;
    if(!FIDvec.empty()) controlBreakPoint(*os);
    else *os << "Vector is empty" << endl;
    *os << DIVIDER << endl;
    
    stable_sort(FIDvec.begin(), FIDvec.end(), FileID::bySize);
    
    start = FIDvec.begin();
    end = FIDvec.end();
    walker = FIDvec.begin();
    
    for(;;){
        for(;;){
            if(start->getSize() != walker->getSize()) break;
            
            if(start->getSize() == walker->getSize()) ++walker;
            else break;
        }
        
        if(end - start > 1){
            checkDups();
            start = walker;
        }
        else{
            start = walker; //Block done
        }
        if(start == end) break;
    }
}

void Sweeper::travel(char* pathName, char* simple, string tabs){
    DIR* dirOpen = opendir(simple);
    chdir(simple);

    Stats s;
    
    for(;;){
        Direntry* dp = (Direntry*)readdir(dirOpen);
        
        if(dp == NULL){
            chdir("..");
            return;
        }
        if(p.v){
            *os << tabs;
            switch (dp->type()) {
                case DT_DIR:
                    *os << "Directory: ";
                    break;
                case DT_LNK:
                    *os << "Link: ";
                    break;
                case DT_REG:
                    *os << "File: ";
                    break;
                default:
                    break;
            }
            *os << dp->name() << " " << dp->ino() << endl;
        }
        if(dp->d_type == DT_DIR){
            if((strcmp(dp->name(), ".") != 0) && (strcmp(dp->name(), "..") != 0)){
                travel(concatPath(pathName, dp->name(), false), dp->name(), tabs + "\t");
            }
        }
        else if(dp->d_type == DT_REG){
            lstat(dp->name(), &s);
            FIDvec.push_back(FileID(concatPath(pathName, dp->name(), true), s.ino(), s.size(), s.nlink()));
        }
    }
}

char* Sweeper::getSimple(char* p){
    string temp = p;
    temp.erase(temp.find_last_of('/'));
    temp.erase(0, temp.find_last_of('/') + 1);
    char* simp = new char[PATH_MAX];
    strcpy(simp, temp.c_str());
    return simp;
}

char* Sweeper::concatPath(char* p, char* simple, bool file){
    string tempP = p;
    string tempS = simple;
    if(!file) tempP += tempS + "/";
    else tempP += tempS;
    char* product = new char[PATH_MAX];
    strcpy(product, tempP.c_str());
    return product;
}

void Sweeper::controlBreakPoint(ostream& out){
    ino_t control = FIDvec[0].getInode();
    out << "I-node = " << control << " links " << FIDvec[0].getNumLinks() << " size " << FIDvec[0].getSize() << " bytes" << endl;
    for (unsigned x = 0; x < FIDvec.size(); ++x){
        if(FIDvec[x].getInode() != control){
            out << "\n";
            control = FIDvec[x].getInode();
            out << "I-node = " << control << " links " << FIDvec[x].getNumLinks() << " size " << FIDvec[x].getSize() << " bytes" << endl;
        }
        FIDvec[x].print(out);
    }
}

void Sweeper::checkDups(){
    bool same;
    
    for(vector<FileID>::iterator scan = start + 1; scan < walker - 1; ++scan){
        if(scan->getInode() != start->getInode()){
            same = false;
            break;
        }
        else if(scan->getInode() == start->getInode()) same = true;
    }
    
    if(!same){
        for(vector<FileID>::iterator x = start; x < walker; ++x){
            x->calcSHA256(x->getPath());
        }
        
        stable_sort(start, walker - 1, FileID::byFPrint);
        printDups(*os);
    }
    else if(same){
        for(vector<FileID>::iterator x = start; x < walker; ++x){
            *os << "Link: " << x->getPath() << endl;
        }
    }
}

void Sweeper::printDups(ostream& out){
    unsigned char *control = start->getFPrint();
    out << "\nFingerprint: " << control << endl;
    for (vector<FileID>::iterator x = start; x < walker; ++x){
        if(strcmp((char*)x->getFPrint(), (char*)control) < 0){
            out << "\n";
            control = x->getFPrint();
            out << "Fingerprint: " << control << endl;
        }
        x->print(out);
    }
}

