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
    ~Mutex(){pthread_mutex_destroy(&mutex);};
    void lock(){pthread_mutex_lock(&mutex);};
    void unlock(){pthread_mutex_unlock(&mutex);};
};

class Counter{
    Mutex mutex;
    unsigned int count;
public:
    Counter(): count(0){};
    void operator++(int){
        mutex.lock();
        count++;
        mutex.unlock();
    };
    void operator--(int){
        mutex.lock();
        count--;
        mutex.unlock();
    };
    unsigned int get(){
        mutex.lock();
        unsigned int count_t = count;
        mutex.unlock();
        return count_t;
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
            UNLOCK(&head);
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
            Node *pred= &head, *curr= nullptr;
            LOCK(pred);
            curr=head.next;
            LOCK(curr);
            bool retval=false;
                while (pred){
                    //Case 1: reach lists end --> insert to tail
                    //Case 2: need to replace head (pred is dummy and data<curr)
                    //Case 3: normal --> prev<data<curr

                    //   curr is tail       pred is dummy
                    if (curr == nullptr || ((pred == &head || data > pred->data) && data < curr->data)) {
                        retval= pred->insert_after(data);
                        break;
                    }
                    else if (data == curr->data) {
                        break;
                    }
                    hand_over_hand(&pred, &curr);
                }

            if(retval) {
                size++;
                __insert_test_hook();
            }
            UNLOCK(pred);
            UNLOCK(curr);
            return retval;
        }

        /**
         * Remove the node that its data equals to @param value
         * @param value the data to lookup a node that has the same data to be removed
         * @return true if a matched node was found and removed and false otherwise
         */
        bool remove(const T& value) {
            Node *pred= &head, *curr= nullptr;
            LOCK(pred);
            curr=head.next;
            LOCK(curr);
            while (curr) {
                if (curr->data == value) {
                    pred->next = curr->next;
                    UNLOCK(curr);
                    delete curr;
                    __remove_test_hook();
                    size--;
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
            return size.get();
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

        bool isSorted(){
            Node *pred= &head, *curr= nullptr;
            LOCK(pred);
            curr=head.next;
            LOCK(curr);
            while (curr) {
                if(pred!=&head && pred->data>curr->data)
                    return false;
                hand_over_hand(&pred, &curr);

            }
            UNLOCK(pred);
            return true;
        }

        static void hand_over_hand(List<T>::Node **pred, List<T>::Node **cur){
            UNLOCK(*pred);
            *pred=*cur;
            *cur=(*cur)->next;
            LOCK(*cur);
        };


		// Don't remove
        virtual void __insert_test_hook() {}
		// Don't remove
        virtual void __remove_test_hook() {}

    private:
        Node head; //dummy
        Counter size;
};


#endif //THREAD_SAFE_LIST_H_