#include "Barrier.h"

Barrier::Barrier(unsigned int num_of_threads) :n(num_of_threads),count(0) {
    sem_init(&mutex,0,1);
    sem_init(&barrier,0,0);
    //sem_init(&barrier2,0,0);
}

void Barrier::wait() {
    sem_wait(&mutex);
    count +=1;
    if(count == n) sem_post(&barrier);
    sem_post(&mutex);

    sem_wait(&barrier);
    sem_post(&barrier);

    //critical point

    sem_wait(&mutex);
    count -=1;
    if(count == 0) sem_wait(&barrier);
    sem_post(&mutex);
}

Barrier::~Barrier() {
    sem_destroy(&mutex);
    sem_destroy(&barrier);
    //sem_destroy(barrier2);
}
