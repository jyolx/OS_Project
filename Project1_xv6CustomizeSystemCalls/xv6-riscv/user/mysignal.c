#include "kernel/types.h"
#include "user.h"

int main() 
{
    int pid = fork();
    if (pid == 0) 
    {
        printf("Child process\n\n");
        int i = 0;
        while (1) 
        {
            printf("Child running\n");
            if (i == 5) 
            {
                printf("Child raising SIGKILL\n");
                raise(SIGKILL); // Mark SIGKILL for child
            }
            sleep(1);
            i++;
        }
    } 
    else 
    {
        sleep(6); // Wait for the child to reach the point where it raises SIGKILL
        printf("Child killed\n");
    }
    exit(0);
}