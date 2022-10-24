#include  <iostream>
#include  <stdlib.h>
#include  <cstring>
#include  <errno.h>
#include  <sys/types.h>
#include  <unistd.h>
#include  <sys/socket.h>
#include  <netdb.h>
#include  <signal.h>
#include  <pthread.h>
#include  "communication.h"
#include  "worker.h"
#include  "mutexDict.h"

using namespace std;

int fd;
extern ExecQueue *myqueue;
extern MutexDict *mydict;

void myhandler(int signum) {
    delete myqueue;
    delete mydict;
    close(fd);
}

int main( int argc, char* argv[] ) {

    signal( SIGINT, myhandler );  

    int port, thread_pool_size, queue_size, block_size;
    for ( int i = 0 ; i < argc ; i++ ) {
        if ( strcmp(argv[i], "-p") == 0 ) port = atoi(argv[i+1]); 
        if ( strcmp(argv[i], "-s") == 0 ) thread_pool_size = atoi(argv[i+1]); 
        if ( strcmp(argv[i], "-q") == 0 ) queue_size = atoi(argv[i+1]); 
        if ( strcmp(argv[i], "-b") == 0 ) block_size = atoi(argv[i+1]); 
    }

    myqueue = new ExecQueue();  // execution queue
    myqueue->set_maxsize(queue_size);

    mydict = new MutexDict();   // mutex dictionary

    int sockfd;
    struct sockaddr_in server, client;
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;

    if( (sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) {                  // socket
        perror("spcket");
        exit(1);
    }
    int enable = 1;
    if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT,  &enable, sizeof(int)) < 0 ) {    // make port reusable
        perror("setsockopt");
        exit(1);
    }

    fd = sockfd;

    server.sin_family = AF_INET;       
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);   

    if ( bind(sockfd, serverptr, sizeof(server)) < 0 ) {                    // bind
        perror("bind");
        exit(1);
    } 

    if ( listen(sockfd, 10) < 0 ) {                                         // listen
        perror("listen");
        exit(1);
    }

    while( thread_pool_size-- ) {
        pthread_t workthread;
        if ( pthread_create(&workthread, 0, work, &block_size) != 0 ) {   // create worker thread, run function work in worker.h
            cout << "pthread_create: " << strerror(errno) << endl;
            exit(1);
        }  
        if (  pthread_detach(workthread) != 0 ) {   // mark the worker thread as detached
            cout << "pthread_detach: " << strerror(errno) << endl;
            exit(1);
        }
    }

    int newsockfd;
    while (1) {
        socklen_t clientlen = sizeof(client);
        if( (newsockfd = accept(sockfd, clientptr, &clientlen)) < 0 ) {     // accept
            perror("accept");
            exit(1);
        }
        cout << "accepted, fd: " << newsockfd << endl;

        // create mutex for this socket
        pthread_mutex_t mtx;
        pthread_mutex_init(&mtx, 0);
        mydict->set_mtx(newsockfd, &mtx);  // push it in mutex dictionary
     
        pthread_t comthread;
        if ( pthread_create(&comthread, 0, communicate, &newsockfd) != 0 ) {    // create communication thread, run function communicate in communication.cpp
            cout << "pthread_detach: " << strerror(errno) << endl;
            exit(1);
        }  
    }
    exit(0);
    
}