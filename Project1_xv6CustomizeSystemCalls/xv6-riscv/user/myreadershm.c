// User program to read from shared memory

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/shm.h"

#define SHM_SIZE 4096  // Size of the shared memory segment

int main() {
    int shmid;
    char *shm_ptr;
    char buffer[100];

    // Get shared memory segment
    shmid = shmget(1234, SHM_SIZE, 0);  // Use the same key as the writer
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
    int i;
    for (i = 0; shm_ptr[i] != '*'; i++) {
        buffer[i] = shm_ptr[i];
    }
    buffer[i]='\0';
    // Read from shared memory and print the message
    printf("Reader read: %s\n", buffer);

    // Detach from shared memory
    if (shmdt(shmid) < 0) {
        printf("shmdt failed\n");
        exit(0);
    }
    // Delete shared memory region
    if(shmctl(shmid,IPC_RMID)<0)
    {
        printf("shmctl failed\n");
        exit(0);
    }

    exit(0);
}
