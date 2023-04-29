#include<stdio.h>
#include <iostream> //

#include "rwlock.h"

//Implement all methods of RWLock defined in rwlock.h
//Your solution for  Task 3 
//TODO

RWLock::RWLock(){
    AR = 0;
    WR = 0;
    AW = 0;
    WW = 0;
    okToRead = new Condition("okToRead");
    okToWrite = new Condition("okToWrite");
    lock = new Lock("lock");
 }

RWLock::~RWLock(){
    okToRead->~Condition();
    okToWrite->~Condition();
    lock->~Lock();
 }

void RWLock::startRead(){
    int status;
    lock->Acquire();
    while ((AW + WW) > 0) { // Is it safe to read?
        WR++; // No-> Writers exist
        okToRead->Wait(lock);
        WR--; // No longer waiting
    }
    AR++; // Now we are active!
    lock->Release();
 }
void RWLock::doneRead(){
    int status;
    lock->Acquire();
    AR--; // No longer active
    if (AR == 0 && WW > 0) // No other active readers
        okToWrite->Signal(lock); // Wake up one writer
    lock->Release();
 }
void RWLock::startWrite(){
    int status;
    lock->Acquire();
    while ((AW + AR) > 0) { // Is it safe to write?
        WW++; // No. Active users exist
        okToWrite->Wait(lock);
        WW--; // No longer waiting
    }
    AW++; // Now we are active!
    lock->Release();
 }
void RWLock::doneWrite(){
    int status;
    lock->Acquire();
    AW--; // No longer active
    if (WW > 0){ // Give priority to writers
        okToWrite->Signal(lock); // Wake up one writer
    } else if (WR > 0) { // Otherwise, wake reader
        okToRead->Broadcast(lock); // Wake all readers
    }
    lock->Release();
 }
