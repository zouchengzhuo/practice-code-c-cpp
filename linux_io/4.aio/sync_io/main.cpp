#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include "kernel_read.h"
#include "glibc_read.h"

int main(){
    //ReadBySyscallOpen();
    //PreadBySyscallOpen();
    //ReadvBySyscallOpen();
    PreadvBySyscallOpen();
    //ReadByFOpen();
}