#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    /*
    int p[2];
    char msg[5];
    msg[4] = 0;
    pipe(p);

    int fork_result = fork();
    int pid = getpid();
    if (fork_result == 0)
    {
        read(p[0], msg, 4);
        printf("child %d: got %s\n", pid, msg);
        write(p[1], "pong", 4);
    }
    else
    {
        write(p[1], "ping", 4);
        wait(0);
        read(p[0], msg, 4);
        printf("parent %d: got %s\n", pid, msg);
    }
    exit(0);
     */

    int p[2][2];
    pipe(p[0]);
    pipe(p[1]);
    char msg[5];
    msg[4] = 0;

    int fork_result = fork();
    int pid = getpid();
    if (fork_result == 0)
    {
        close(p[0][0]);
        close(p[1][1]);
        read(p[1][0], msg, 4);
        printf("child %d: got %s\n", pid, msg);
        write(p[0][1], "pong", 4);
    }
    else
    {
        close(p[0][1]);
        close(p[1][0]);
        write(p[1][1], "ping", 4);
        read(p[0][0], msg, 4);
        printf("parent %d: got %s\n", pid, msg);
    }
    exit(0);
}