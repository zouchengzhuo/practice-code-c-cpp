#include <set>
#include <sys/select.h>

void AddFd(fd_set &origin_set, std::set<int> &fds, int &max_fd, int &fd){
    FD_SET(fd, &origin_set);
    if(fd > max_fd) max_fd = fd;
    fds.insert(fd);
};

void RemoveFd(fd_set &origin_set, std::set<int> &fds, int &max_fd, std::set<int>::iterator &iter){
    int fd = *iter;
    FD_CLR(fd, &origin_set);
    iter = fds.erase(iter);
    if(fd >= max_fd && !fds.empty()){
        max_fd = *fds.rbegin();
        printf("set max_fd to: %d \n", max_fd);
    }
};

void PrintBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    printf(" ");
}

void PrintSet(fd_set *fdset)
{
    int s = __FD_SETSIZE / __NFDBITS;
    int i;
    for (i = 0; i < s; i++)
    {
        //printf("%lu ", (__fd_mask)fdset->fds_bits[i]);
        PrintBits(sizeof(__fd_mask), (unsigned __fd_mask*)&fdset->fds_bits[i]);
    }
    printf("\n");
}