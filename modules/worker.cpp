#include  "worker.h"

extern MutexDict *mydict;
extern ExecQueue *myqueue;
extern pthread_cond_t cond_nonfull;

void *work(void *ptr) {
    cout << "new worker thread" << endl;
    int block = *((int *) ptr);
    while (1) {
        struct file *file = myqueue->pop();
        pthread_cond_signal(&cond_nonfull);

        pthread_mutex_t *mtx = mydict->get_mtx(file->sockfd);
        pthread_mutex_lock(mtx);

        char name[1024];
        memset(name, 0, 1024);
        memcpy(name, file->name, strlen(file->name));
        // send the filename
        send(file->sockfd, name, 1024);

        char buf[block];
        memset(buf, 0, block);
        
        if ( strcmp(file->name, "end") == 0 ) { // if this is the dummy node, the file->size contains the number of the 
            sprintf(buf, "%d", file->size);     // directory's files so i want to send it as it is
            send(file->sockfd, buf, 4);
            pthread_mutex_unlock(mtx);
            continue;
        }

        int size = (file->size/block+1)*block;
        sprintf(buf, "%d", size);
        // send the filesize
        send(file->sockfd, buf, 4);


        int fd;
        if ( (fd = open(file->name, 0666)) < 0 ) {  // open file to read its contents
            perror("open");
            exit(1);
        }

        memset(buf, 0, block);
        int n;
        // send contents
        while ( (n = read(fd, buf, block)) ) {
            send(file->sockfd, buf, block);
            memset(buf, 0, block);
        }
        if ( n < 0 ) {
            perror("read");
            exit(1);
        }

        pthread_mutex_unlock(mtx);

        if ( close(fd) < 0 ) {
            perror("close");
            exit(1);
        }
    }
}


void send(int sockfd, char *buf, int bytes) {
    //cout << "ID   " << pthread_self()  << "   BUF   " << buf << endl;
    if ( write(sockfd, buf, bytes) < 0 ) {
        perror("worker: write");
        exit(1);
    }
}