//
//  FileID.hpp
//  Project5
//
//  Created by Alec Shackett on 10/8/17.
//  Copyright © 2017 Alec Shackett. All rights reserved.
//

#ifndef FileID_hpp
#define FileID_hpp

#include <stdio.h>
#include "tools.hpp"
#include <openssl/sha.h>

#define BUFSIZE 4096

class FileID{
private:
    char* path;
    ino_t inode;
    off_t size;
    nlink_t numLinks;
    unsigned char FPrint[SHA256_DIGEST_LENGTH];
public:
    FileID(char* p, ino_t iNum, off_t s, nlink_t links) : path(p), inode(iNum), size(s), numLinks(links){}
    void print(ostream& out){out << "\t" << path << endl;}
    void printFPrint(ostream& out){out << "\t" << path << " " << FPrint << endl;}
    char* getPath(){return path;}
    ino_t getInode(){return inode;}
    off_t getSize(){return size;}
    unsigned char* getFPrint(){return FPrint;}
    int getNumLinks(){return numLinks;}
    static bool less(const FileID& a, const FileID& b) {return (a.inode < b.inode);}
    static bool bySize(const FileID& a, const FileID& b) {return (a.size < b.size);}
    static bool byFPrint(const FileID& a, const FileID& b) {return (a.FPrint < b.FPrint);}
    void calcSHA256(char* tpath);
};

#endif /* FileID_hpp */
