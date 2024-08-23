#include "kernel/types.h" 
#include "kernel/stat.h" 
#include "user/user.h"  

int main(int argc, char *argv[]) {
    int p[2];
    char buf[1]; 

    pipe(p);

    if (fork() == 0) { // Child process
        if (read(p[0], buf, 1) != 1) {
            fprintf(2, "Child: read failed\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());

        if (write(p[1], buf, 1) != 1) {
            fprintf(2, "Child: write failed\n");
            exit(1);
        }

        close(p[0]);
        close(p[1]);
        exit(0);
    } else { // Parent process
        buf[0] = '$';
        
        if (write(p[1], buf, 1) != 1) {
            fprintf(2, "Parent: write failed\n");
            exit(1);
        }

        wait(0);

        if (read(p[0], buf, 1) != 1) {
            fprintf(2, "Parent: read failed\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());

        close(p[0]);
        close(p[1]);
        exit(0);
    }
}