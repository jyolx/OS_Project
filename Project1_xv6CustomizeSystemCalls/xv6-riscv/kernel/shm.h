#define MAX_SHM_SEGMENTS 10  //Maximum shm segments
#define SHM_SIZE 4096       //Maximum shm size=4KB
#define SHM_BASE 0x20000000
#define IPC_STAT 1
#define IPC_RMID 2

//Structure for each SHM segment
struct shm_segment
{
    int id;
    int key;
    int size;
    int ref_count;      //Number of processes using this shm segment
    int flags;
    void *addr;
};
