#ifndef TESTINLINE_H
#define TESTINLINE_H

#include <iostream>
#include <ctime>
#include <unistd.h>

int testTime(const int &a, const int &b){
//int testTime(int a, int b){
    return a*b;
}
inline int testInlineTIme(const int &a, const int &b){
//inline int testInlineTIme(int a, int b){
    return a*b;
};

#endif