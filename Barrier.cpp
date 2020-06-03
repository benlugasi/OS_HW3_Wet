#include "Barrier.h"

Barrier::Barrier(unsigned int num_of_threads) {

}

void Barrier::wait() {
    mutex.wait();
    count +=1;
    if(count == n) barrier.signal();
    mutex.signal();

    barrier.wait();
    barrier.signal();

    //critical point

    mutex.wait();
    count -=1;
    if(count == 0) barrier.wait();
    mutex.signal();
}

Barrier::~Barrier() {

}
