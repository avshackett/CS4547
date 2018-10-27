//
//  main.cpp
//  Project5
//
//  Created by Alec Shackett on 10/6/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#include <iostream>
#include "Sweeper.hpp"
#include "tools.hpp"

int main(int argc, char* const argv[]){
    banner();
    Sweeper s = Sweeper(argc, argv);
    s.run();
    bye();
}
