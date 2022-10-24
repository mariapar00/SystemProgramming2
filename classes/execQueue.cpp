#include  "execQueue.h"

ExecQueue *myqueue;
pthread_mutex_t mtx;   // access to the queue
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;

ExecQueue::ExecQueue() {
    size = 0;
    pthread_mutex_init(&mtx, 0);
    pthread_cond_init(&cond_nonempty, 0);
    pthread_cond_init(&cond_nonfull, 0);
}

ExecQueue::~ExecQueue() {
    struct file *front;
    while ( (front = equeue.front()) != NULL ) {
        equeue.pop();
        delete front;
    }}

void ExecQueue::set_maxsize(int size) {
    max_size = size;
}

int ExecQueue::get_size()const {
    return size;
}

int ExecQueue::get_maxsize()const {
    return max_size;
}

struct file *ExecQueue::qfront()const {
    return equeue.front();
}

void ExecQueue::push(struct file *myfile) {
    pthread_mutex_lock(&mtx);
    while ( size >= max_size ) {
        cout << "queue found full" << endl;
        pthread_cond_wait(&cond_nonfull, &mtx); // waiting for a worker thread to free space
    }
    equeue.push(myfile);
    size++;

    pthread_mutex_unlock(&mtx);
}

struct file *ExecQueue::pop() {
    pthread_mutex_lock(&mtx);
    while ( size <= 0 ) {
        cout << "queue found empty" << endl;
        pthread_cond_wait(&cond_nonempty, &mtx); // waiting for a communication thread to insert element
    }
    struct file *myfile = qfront();
    equeue.pop();
    size--;

    pthread_mutex_unlock(&mtx);
    return myfile;
}