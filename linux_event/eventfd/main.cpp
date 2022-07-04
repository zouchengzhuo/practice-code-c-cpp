#include <iostream>
#include <sys/eventfd.h>
#include <unistd.h>
#include <inttypes.h> /* Definition of PRIu64 & PRIx64 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> /* Definition of uint64_t */

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

int main(int argc, char *argv[])
{
    int efd;
    uint64_t u;
    ssize_t s;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <num>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    //默认阻塞模式，读取时一次性将counter清零，如果已经是零了则让进程阻塞
    //efd = eventfd(0, 0);
    //非阻塞，信号量模式
    efd = eventfd(0, EFD_SEMAPHORE|EFD_NONBLOCK);
    if (efd == -1)
        handle_error("eventfd");

    switch (fork())
    {
    case 0:
        for (int j = 1; j < argc; j++)
        {
            printf("Child writing %s to efd\n", argv[j]);
            u = strtoull(argv[j], NULL, 0);
            /* strtoull() allows various bases */
            s = write(efd, &u, sizeof(uint64_t));
            if (s != sizeof(uint64_t))
                handle_error("write");
        }
        printf("Child completed write loop\n");

        exit(EXIT_SUCCESS);

    default:
        while (1)
        {
            sleep(1);
            printf("Parent about to read, u: %d\n", u);
            s = read(efd, &u, sizeof(uint64_t));
            printf("Parent read s:%d, u:%d\n", s, u);
            if (s != sizeof(uint64_t)){
                if(errno == EAGAIN){
                    exit(EXIT_SUCCESS);
                }
                handle_error("read");
            }
            printf("Parent read %" PRIu64 " (%#" PRIx64 ") from efd\n", u, u);
        }
    case -1:
        handle_error("fork");
    }
}