#ifndef BARRIER_H_
#define BARRIER_H_

#include <semaphore.h>

class Barrier {
    sem_t mutex;
    sem_t barrier;
    //sem_t barrier2;
    unsigned int n;
    unsigned int count;
public:
    Barrier(unsigned int num_of_threads);
    void wait();
    ~Barrier();

	// TODO: define the member variables
	// Remember: you can only use semaphores!
};

#endif // BARRIER_H_

