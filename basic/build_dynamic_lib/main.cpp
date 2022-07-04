#include <iostream>
#include <unistd.h>
#include "libtest.h"

int main(){
    Test::MyRandom myRandom;
    for(int i=0;i<10;i++){
        sleep(1);
        std::cout << "rand:" << myRandom.getRandom100() <<std::endl;
    }
    return 0;
}