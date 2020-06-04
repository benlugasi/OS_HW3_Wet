#ifndef BARRIER_H_
#define BARRIER_H_

#include <semaphore.h>
typedef unsigned int uint;

class Fence{
    sem_t semaphore;
    sem_t* p_mutex;
    uint counter;
    uint n;
public:
    Fence(sem_t* p_mutex, uint n);
    ~Fence();
    void open();
    void close();
    bool is_open();
};

class Barrier {

    sem_t mutex;
    uint n;
    Fence fence1;
    Fence fence2;

public:
    Barrier(unsigned int num_of_threads);
    void wait();
    ~Barrier();
    bool is_close();
};

#endif // BARRIER_H_

