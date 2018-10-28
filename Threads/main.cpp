//
//  main.cpp
//  Project9
//
//  Created by Alec Shackett on 11/27/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#include <iostream>
#include "tools.hpp"
#include "Shared.hpp"
#include "Mom.hpp"

bool startFlag = false;
bool quitFlag = false;
sigset_t emptySet;

void sigUsr1Handler(int sig){
    startFlag = true;
}

int main(int argc, const char * argv[]) {
    srand(time(NULL));
    banner();
    Mom m = Mom();
    m.run();
}
