//
//  FileID.cpp
//  Project7
//
//  Created by Alec Shackett on 11/6/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#include <stdio.h>
#include "FileID.hpp"

void FileID::calcSHA256(char* tpath){
    ifstream in (tpath);
    
    if (!in) fatal("can't open %s for reading", tpath);
    
    SHA256_CTX ctx;   // A structure of state info used in computing SHA.
    
    if (!SHA256_Init(&ctx)) fatal("SHA256_Init failure");
    
    // Fingerprinting ====================================================
    char buf[BUFSIZE];
    
    for (;;) {
        in.read( buf, BUFSIZE );
        size_t bytes_read = in.gcount();
        if (bytes_read == 0) break;
        // Update the state by processing the next block of data.
        if (!SHA256_Update( &ctx, buf, bytes_read) ) fatal("SHA256_Update failure");
    }
    
    // Terminate SHA algorithm and extract the answer from the context.
    if (!SHA256_Final(FPrint, &ctx)) fatal("SHA256_Final failure");
}
