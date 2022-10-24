#include  <iostream>
#include  <stdlib.h>
#include  <cstring>
#include  <errno.h>
#include  <sys/types.h>
#include  <unistd.h>
#include  <sys/socket.h>
#include  <netdb.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#include  <time.h>
#include  <limits.h>
#include  "folderCreation.h"

using namespace std;

int generate_no();

void read_socket(int, char *, int);

int main( int argc, char* argv[] ) {
    srand(time(0));  // the current time is the seed

    int server_port;
    char *server_ip, *directory;
    for ( int i = 0 ; i< argc ; i++ ) {
        if ( strcmp(argv[i], "-i") == 0 ) server_ip = argv[i+1];
        if ( strcmp(argv[i], "-p") == 0 ) server_port = atoi(argv[i+1]);
        if ( strcmp(argv[i], "-d") == 0 ) directory = argv[i+1];
    }

    //create local file system
    int num = generate_no();
    char name[34];
    sprintf(name, "ClientsFileSystems/FileSystem%d", num);
    create_dir(name);
    cout << "created " << name << endl;

    int sockfd;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;

    if ( (sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) {         // socket
        perror("socket");
        exit(1);
    }

    server.sin_family = AF_INET;      
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(server_port); 

    if ( connect(sockfd, serverptr, sizeof(server)) < 0 ) {         // connect
        perror("connect");
        exit(1);
    }
    cout << "connected" << endl;

    if ( write(sockfd, directory, strlen(directory)+1) < 0 ) {      // write
        perror("client: write");
        exit(1);
    }
    
    int counter = 0;                                // indicates the number of files copied
    int limit = INT_MAX;                            // limit is maximum until the number of the directory's files is known
    while ( counter < limit ) {
        char *buf = new char[1024];                 // buffer for path to file
        read_socket(sockfd, buf, 1024);             // read 1024 bytes from socket
        if ( strcmp(buf, "end") == 0 ) {            // if filename is "end" then the "package" is the dummy 
            char *filesum = new char[4];                    
            read_socket(sockfd, filesum, 4);        // read 4 bytes from socket which mark the total files of the directory
            limit = atoi(filesum);                  // limit now equals to the number of the directory's files and
            delete filesum;                         // the loop will continue until counter reaches this number
            continue;
        }
        
        adjust_filename(&buf, directory);           // if "directory" is a relative path i don't want to copy the previous
                                                    // directories so i adjust the name
        char *path = new char[strlen(buf)+strlen(name)+2];
        strcpy(path, name);                         // "name" is: "ClientsFileSystems/FileSystem<num>"
        strcat(path, "/");
        strcat(path, buf);                          // "path" is a relative path to the client's file system
        delete buf;
        
        create_path(path);                          // create path
 
        char *size = new char[4];
        read_socket(sockfd, size, 4);               // read 4 bytes from socket
        char *contents = new char[atoi(size)+1];    // contents is of size "size", which is a multiple of block_size
    
        read_socket(sockfd, contents, atoi(size));  // read "size" bytes from socket
       
        paste_contents(path, contents);             // paste the contents of the file into the already created new file
        delete size;        
        delete path;
        delete contents;
        counter++;                                  // counter increased as we copied one more of the directory's files
    }

    cout << "exiting" << endl;
    close(sockfd);
    exit(0);
}


// generate random number from 0 to 1000 
int generate_no() {
    return rand() % 1000;
}


// read readsize bytes from socket
void read_socket(int sockfd, char *buf, int readsize) {    
    while ( (readsize -= read(sockfd, buf, readsize)) > 0 )   // read might not read as many bytes as I asked her to from the start :/
        buf += readsize;
}