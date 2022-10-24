#include  "mutexDict.h"

MutexDict *mydict;

MutexDict::MutexDict() {

}

MutexDict::~MutexDict() {

}


void MutexDict::set_mtx(int sockfd, pthread_mutex_t *mtx) {
    sock_mtxs.insert({sockfd, mtx});
}

pthread_mutex_t *MutexDict::get_mtx(int sockfd) {
    auto x = sock_mtxs.find(sockfd);
    return x->second;
}