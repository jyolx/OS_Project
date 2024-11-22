// kernel/shm.c
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "shm.h"

// Shared memory segments array
static struct shm_segment shm_segments[MAX_SHM_SEGMENTS];

// Initialize shared memory segments (this is called once in kernel initialization)
void init_shm_segments() 
{
    for (int i = 0; i < MAX_SHM_SEGMENTS; i++) 
    {
        shm_segments[i].id=100+i;
        shm_segments[i].key = -1;  // Mark as unused
        shm_segments[i].size = 0;
        shm_segments[i].ref_count = 0;
        shm_segments[i].addr = 0;
    }
}

// sys_shmget: Create or get a shared memory segment
int shmget(int key,int size,int flags) 
{
    for (int i = 0; i < MAX_SHM_SEGMENTS; i++) 
    {
        if (shm_segments[i].key == key) 
        {
            if ((shm_segments[i].flags & flags) != flags) 
            {
                return -1;  // Permissions mismatch
            }
            return shm_segments[i].id;
        }
    }

    // No existing segment, create a new one
    for (int i = 0; i < MAX_SHM_SEGMENTS; i++) 
    {
        if (shm_segments[i].key == -1) 
        {
            shm_segments[i].key = key;
            // Max allowed size is 4kB, one page
            if(size>SHM_SIZE)
                return -1;
            shm_segments[i].size = size;
            shm_segments[i].addr = kalloc();
            return shm_segments[i].id;
        }
    }
    return -1;  // No space for a new segment
}

// sys_shmat: Attach shared memory segment to the process address space
int shmat(int shmid) 
{

    struct proc *p = myproc();
    for (int i = 0; i < MAX_SHM_SEGMENTS; i++) 
    {
        if (shm_segments[i].id == shmid) 
        {
            shm_segments[i].ref_count++;  

            uint64 pa = (uint64)shm_segments[i].addr;  

            // Iterate through the entire address space to find a free page
            for (uint64 va = 0; va < MAXVA; va += PGSIZE) 
            { 
                pte_t *pte = walk(p->pagetable, va, 0);
                if (*pte == 0) 
                { 
                    // If there's no mapping, this page is free
                    // Map the shared memory segment to the free virtual address
                    mappages(p->pagetable, va, SHM_SIZE, pa, PTE_R | PTE_W | PTE_U);
                    
                    p->shm_va = va;
                    return va; 
                }
            }
            // If no free pages were found, return an error
            return -1;
        }
    }
    return -1;  // Segment not found
}

// sys_shmdt: Detach shared memory segment from the process address space
int shmdt(int shmid) {
    struct proc *p = myproc();
    
    // Find the shared memory segment corresponding to the shmid
    for (int i = 0; i < MAX_SHM_SEGMENTS; i++) {
        if (shm_segments[i].id == shmid) {
            // Unmaping the page corresponding to shm
            uvmunmap(p->pagetable, p->shm_va, 1, 0);
            shm_segments[i].ref_count--;
            
            return 0;
        }
    }
    
    return -1; // Shared memory segment not found
}

// sys_shmctl: Control operations on shared memory (e.g., remove or get stats)
int shmctl(int shmid,int cmd) {
    // Find the shared memory segment corresponding to the shmid
    for (int i = 0; i < MAX_SHM_SEGMENTS; i++) {
        if (shm_segments[i].id == shmid) {
            switch (cmd) {
                case IPC_RMID:
                    // Remove shared memory segment
                    // Ensure that no process is using the segment
                    if (shm_segments[i].ref_count == 0) {
                        kfree((void *)shm_segments[i].addr);
                        shm_segments[i].key = -1;
                        return 0;
                    }
                    return -1; // Segment is still in use by some process

                default:
                    return -1; // Invalid command
            }
        }
    }
    return -1; // Segment not found
}

