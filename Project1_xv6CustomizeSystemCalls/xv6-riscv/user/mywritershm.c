// User program to write to shared memory

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

#define SHM_SIZE 4096  // Size of the shared memory segment

int main() {
    int shmid;
    char *shm_ptr;
    char message[300];

    // Create or get shared memory segment
    shmid = shmget(1234, SHM_SIZE, 0);  // Use a key of 1234, no permissions required for writing
    if (shmid < 0) {
        printf("shmget failed\n");
        exit(0);
    }
    printf("Shmid:%d\n",shmid);

    // Attach to shared memory segment
    shm_ptr = (char *)shmat(shmid);
    if (shm_ptr == (char *)-1) {
        printf("shmat failed\n");
        exit(0);
    }

    printf("Enter your string:");
    gets(message,sizeof(message));
    // Write to shared memory
    int i;
    for (i = 0; message[i] != '\0'; i++) {
        shm_ptr[i] = message[i];
    }
    shm_ptr[i]='*';

    // Detach from shared memory
    if (shmdt(shmid) < 0) {
        printf("shmdt failed\n");
        exit(0);
    }

    printf("Writer wrote: %s\n", message);
    exit(0);
}
