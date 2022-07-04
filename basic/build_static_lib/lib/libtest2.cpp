#include "libtest2.h"

int Test::MyRandom2::getRandom1000(){
    std::srand(std::time(NULL));
    return std::rand() % 1000;
}