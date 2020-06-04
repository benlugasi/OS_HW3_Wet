#ifndef THREAD_SAFE_LIST_H_
#define THREAD_SAFE_LIST_H_

#include <pthread.h>
#include <iostream>
#include <iomanip> // std::setw

using namespace std;

template <typename T>
class List 
{
    public:
        List() : head(new Node(T(), nullptr)) {}
        ~List(){
            head->lock();
            while(head->next)
                remove(head->next->data);
            delete head;
        }

        class Node {
         public:
          T data;
          Node *next;
          pthread_mutex_t mutex;
          explicit Node(T data, Node *next= nullptr) : data(data), next(next){
              pthread_mutex_init(&mutex, NULL);
          }
          ~Node(){
              pthread_mutex_destroy(&mutex);
              unlock();
          };
          Node(Node&)= delete;
          Node& operator=(Node&)= delete;
          void lock(){
              //cout<<"Lock "<<data<<endl;
              pthread_mutex_lock(&mutex);
          };
          void unlock(){
              //cout<<"Unlock "<<data<<endl;
              pthread_mutex_unlock(&mutex);};
          void insert_after(const T& data_in){next= new Node(data_in, next);}
          bool last() const {return next==nullptr;}
          //void remove(Node* pred);

        };

/*
        class ReturnVal : std::exception{
        public:
            bool val;
            explicit ReturnVal(bool val) : val(val){};
        };
        class Success : ReturnVal{
        public:
            Success() : ReturnVal(true){};
        };
        class Failure : ReturnVal{
        public:
            Failure() : ReturnVal(false){};
        };*/

        /**
         * Insert new node to list while keeping the list ordered in an ascending order
         * If there is already a node has the same data as @param data then return false (without adding it again)
         * @param data the new data to be added to the list
         * @return true if a new node was added and false otherwise
         */
        bool insert(const T& data) {
            //cout<<"----Insert "<<data<<endl;
            Node *pred= nullptr, *cur=head;
            cur->lock();
            //try {

                while (true){
                    hand_over_hand(&pred, &cur);

                    //Case 1: reach lists end --> insert to tail
                    //Case 2: need to replace head (pred is dummy and data<cur)
                    //Case 3: normal --> prev<data<cur

                    //   cur is tail       pred is dummy
                    if (cur == nullptr || ((pred == head || data > pred->data) && data < cur->data)) {
                        pred->insert_after(data);
                        __insert_test_hook();
                        if(pred) pred->unlock();
                        if(cur) cur->unlock();
                        return true;
                        //throw ReturnVal(true);
                    }

                    else if (data == cur->data) {
                        if (pred) pred->unlock();
                        if (cur) cur->unlock();
                        return false;
                        //throw ReturnVal(false);
                    }
                }

            /*}
            catch(ReturnVal& r){
                if(pred) pred->unlock();
                if(cur) cur->unlock();
                return r.val;
            }*/

        }

        /**
         * Remove the node that its data equals to @param value
         * @param value the data to lookup a node that has the same data to be removed
         * @return true if a matched node was found and removed and false otherwise
         */
        bool remove(const T& value) {
            Node* pred = head,* curr = head->next;
            pred->lock();
            while (curr) {
                if (curr->data == value) {
                    pred->next = curr->next;
                    delete curr;
                    __remove_test_hook();
                    pred->unlock();
                    return true;
                }
                hand_over_hand(&pred, &curr);
            }
            pred->unlock();
            return false;

        }

        /**
         * Returns the current size of the list
         * @return current size of the list
         */
        unsigned int getSize() {
            unsigned int size = 0;
            Node* pred = head,* curr = head->next;
            pred->lock();
            while(curr){
                size++;
                hand_over_hand(&pred, &curr);
            }
            if(pred) pred->unlock();
			return size;
        }

		// Don't remove
        void print() {
          Node* temp = head->next;
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
        Node* head;
    // TODO: Add your own methods and data members
};

template<typename T>
void List<T>::hand_over_hand(List<T>::Node **pred, List<T>::Node **cur) {
    if(*pred) (*pred)->unlock();
    *pred=*cur;
    *cur=(*cur)->next;
    if(*cur) (*cur)->lock();

}



#endif //THREAD_SAFE_LIST_H_