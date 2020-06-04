#include "Barrier.h"

Barrier::Barrier(uint num_of_threads) : n(num_of_threads), fence1(&mutex, n), fence2(&mutex, n){
    sem_init(&mutex,0,1);
}

void Barrier::wait() {
    fence1.open();
    fence1.close();
    fence2.open();
    fence2.close();
}

Barrier::~Barrier() {
    sem_destroy(&mutex);
}


bool Barrier::is_close(){
    return !fence1.is_open() && !fence2.is_open();
}

/////////////////////////////////////////////////////
///////////////////// Fence API /////////////////////
/////////////////////////////////////////////////////


Fence::Fence(sem_t* p_mutex, uint n) : p_mutex(p_mutex), counter(0), n(n){
    sem_init(&semaphore, 0, 0);
}

Fence::~Fence() {
    sem_destroy(&semaphore);
}

void Fence::open() {
    //wait to the last thread and open
    sem_wait(p_mutex);
    counter++;
    if(counter == n) sem_post(&semaphore);
    sem_post(p_mutex);

    sem_wait(&semaphore);
    sem_post(&semaphore);
}

void Fence::close() {
    //wait to the last thread and close
    sem_wait(p_mutex);
    counter--;
    if(counter == 0) sem_wait(&semaphore);
    sem_post(p_mutex);
}

bool Fence::is_open() {
    int val1;
    sem_getvalue(&semaphore, &val1);
    return val1;
}
