//
//  Stats.hpp
//  Project5
//
//  Created by Alec Shackett on 10/6/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#ifndef Stats_hpp
#define Stats_hpp

#include <stdio.h>
#include <sys/stat.h>
#include "tools.hpp"

class Stats : public stat{
private:
public:
    void print( ostream& out ){
        out << ino() << endl;
        out << size() << endl;
        out << nlink() << endl;}
    ino_t ino(){return stat::st_ino;}
    off_t size(){return stat::st_size;}
    nlink_t nlink(){return stat::st_nlink;}
};

#endif /* Stats_hpp */
