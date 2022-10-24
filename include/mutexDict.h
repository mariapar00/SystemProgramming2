#pragma once
#include  <iostream>
#include  <map>

using namespace std;
 

class MutexDict {
    private:
        map<int, pthread_mutex_t *> sock_mtxs;
    public:
        MutexDict();
        ~MutexDict();

        void set_mtx(int, pthread_mutex_t *);
        pthread_mutex_t *get_mtx(int);
};

