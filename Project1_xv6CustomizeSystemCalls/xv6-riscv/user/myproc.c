// User program to display process info

#include "user.h"

int main(void) 
{
    procinfo();
    if(fork())
    {
        wait(0);
        printf("\nParent process\n");
        procinfo();
    }
    else
    {
        printf("\nChild process\n");
        procinfo();
    }
    return 0;
}