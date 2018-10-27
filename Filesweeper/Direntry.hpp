//
//  Direntry.hpp
//  Project5
//
//  Created by Alec Shackett on 10/8/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#ifndef Direntry_hpp
#define Direntry_hpp

#include <stdio.h>
#include "tools.hpp"
#include <dirent.h>

class Direntry : public dirent{
private:
public:
    Direntry();
    void print(ostream& out){
        out << name() << endl;
        out << ino() << endl;
        out << type() << endl;}
    char* name(){return dirent::d_name;}
    ino_t ino(){return dirent::d_ino;}
    __uint8_t type(){return dirent::d_type;}
};
#endif /* Direntry_hpp */
