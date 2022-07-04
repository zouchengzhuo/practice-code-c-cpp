#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "libaio_read.h"

int main(){
    //ReadByKernelAIO();
    //ReadByKernelAIOSubmitError1();
    //ReadByKernelAIOSubmitError2();
    //ReadByKernelAIOAllocError();
    //BatchReadByLibAIO();
    ReadByKernelAIOAndEventfdAndEpoll();
}