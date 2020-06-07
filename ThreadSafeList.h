#ifndef THREAD_SAFE_LIST_H_
#define THREAD_SAFE_LIST_H_

#include <pthread.h>
#include <iostream>
#include <iomanip> // std::setw

using namespace std;

#define LOCK(node)     if((node)) (node)->mutex.lock()
#define UNLOCK(node)   if((node)) (node)->mutex.unlock()


class Mutex{
    pthread_mutex_t mutex;
public:
    Mutex(){pthread_mutex_init(&mutex, NULL);};
    ~Mutex(){
        unlock();
        pthread_mutex_destroy(&mutex);
    };
    void lock(){
        pthread_mutex_lock(&mutex);
    };
    void unlock(){
        pthread_mutex_unlock(&mutex);
    };
};

template <typename T>
class List 
{
    public:
        List()= default;
        ~List(){
            LOCK(&head);
            while(head.next)
                remove(head.next->data);
        }

        class Node {
         public:
          T data;
          Node *next;
          Mutex mutex;
          explicit Node(T data=T(), Node *next= nullptr) : data(data), next(next){}
          ~Node()= default;
          Node(Node&)= delete;
          Node& operator=(Node&)= delete;
          bool insert_after(const T& data_in){
              try {
                  next = new Node(data_in, next);
              }
              catch(std::bad_alloc& e){
                  return false;
              }
              return true;
          }
        };

        /**
         * Insert new node to list while keeping the list ordered in an ascending order
         * If there is already a node has the same data as @param data then return false (without adding it again)
         * @param data the new data to be added to the list
         * @return true if a new node was added and false otherwise
         */
        bool insert(const T& data) {
            Node *pred= &head, *cur=head.next;
            LOCK(pred);
            LOCK(cur);
            bool retval=false;
                while (pred){
                    //Case 1: reach lists end --> insert to tail
                    //Case 2: need to replace head (pred is dummy and data<cur)
                    //Case 3: normal --> prev<data<cur

                    //   cur is tail       pred is dummy
                    if (cur == nullptr || ((pred == &head || data > pred->data) && data < cur->data)) {
                        retval= pred->insert_after(data);
                        break;
                    }
                    else if (data == cur->data) {
                        break;
                    }
                    hand_over_hand(&pred, &cur);
                }

            if(retval) {
                __insert_test_hook();
                size_lock.lock();
                size++;
                size_lock.unlock();
            }
            UNLOCK(cur);
            UNLOCK(pred);
            return retval;
        }

        /**
         * Remove the node that its data equals to @param value
         * @param value the data to lookup a node that has the same data to be removed
         * @return true if a matched node was found and removed and false otherwise
         */
        bool remove(const T& value) {
            Node* pred = &head,* curr = head.next;
            LOCK(pred);
            LOCK(curr);
            while (curr) {
                if (curr->data == value) {
                    pred->next = curr->next;
                    delete curr;
                    __remove_test_hook();
                    size_lock.lock();
                    size--;
                    size_lock.unlock();
                    UNLOCK(pred);
                    return true;
                }
                hand_over_hand(&pred, &curr);
            }
            UNLOCK(pred);
            return false;

        }

        /**
         * Returns the current size of the list
         * @return current size of the list
         */
        unsigned int getSize() {
            size_lock.lock();
            unsigned int size_t = size;
            size_lock.unlock();
            return size_t;
        }

		// Don't remove
        void print() {
          Node* temp = head.next;
          if (temp == NULL)
          {
            cout << "";
          }
          else if (temp->next == NULL)
          {
            cout << temp->data;
          }
          else
          {
            while (temp != NULL)
            {
              cout << right << setw(3) << temp->data;
              temp = temp->next;
              cout << " ";
            }
          }
          cout << endl;
        }


		// Don't remove
        virtual void __insert_test_hook() {}
		// Don't remove
        virtual void __remove_test_hook() {}

        static void hand_over_hand(List<T>::Node **pred, List<T>::Node **cur);

    private:
        Node head;
        unsigned int size;
        Mutex size_lock;
};

template<typename T>
void List<T>::hand_over_hand(List<T>::Node **pred, List<T>::Node **cur) {
    UNLOCK(*pred);
    *pred=*cur;
    *cur=(*cur)->next;
    LOCK(*cur);

}



#endif //THREAD_SAFE_LIST_H_