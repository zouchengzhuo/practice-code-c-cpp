#include <iostream>
#include <unistd.h>
#include "libtest.h"
#include "libtest2.h"

int main(){
    Test::MyRandom myRandom;
    Test::MyRandom2 myRandom2;
    for(int i=0;i<10;i++){
        sleep(1);
        std::cout << "rand100:" << myRandom.getRandom100() << " rand1000:" << myRandom2.getRandom1000() <<std::endl;
    }
    return 0;
}