#include "libtest.h"

int Test::MyRandom::getRandom100(){
    std::srand(std::time(NULL));
    return std::rand() % 100;
}