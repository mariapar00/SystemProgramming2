#include  "communication.h"

extern ExecQueue *myqueue;
extern pthread_cond_t cond_nonempty;

void *communicate(void *ptr) {
    cout << "new communication thread" << endl;
    int sockfd = *((int *) ptr);
    char *dirname = read_dirname(sockfd);      // read directory name from socket

    int sum = 0;
    content(dirname, sockfd, sum);  // recursively traverse directory to find and save its files as relative paths

    insert_file((char *)"end", sockfd, sum);  // queue entry to symbolize that all files of the directory are sent

    pthread_exit(0);
} 


// read directory name from socket
char *read_dirname(int sockfd) {
    char buf[1024];
    if ( read(sockfd, buf, 1024) < 0 ) {
        perror("server: read");
        exit(1);
    }
    char *name = new char[strlen(buf)+1];
    strcpy(name, buf);
    return name;
}


// recursively traverse directory to find and save its files as relative paths
// sum is number of files found
void content(char *path, int sockfd, int &sum) {
    struct stat pstat;
    if ( stat(path, &pstat) < 0 ) {
        perror("content: stat");
        exit(1);
    }
    if ( (pstat.st_mode & S_IFMT) == S_IFREG ) {    // is regular file
        int size = get_filesize(path);
        insert_file(path, sockfd, size);
        ++sum;
    } else {    // is directory
        DIR* dirp;
        if ( (dirp = opendir(path)) == NULL ) { // open directory
            perror("opendir");
            exit(1);
        }
        while ( struct dirent* entry = readdir(dirp) ) {    // read directory's entries
            if ( strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0 ) continue;

            char *temp = new char[strlen(path)+strlen(entry->d_name)+2];
            strcpy(temp, path);
            strcat(temp, "/");
            strcat(temp, entry->d_name);

            content(temp, sockfd, sum);  
        }
        closedir(dirp);
    }
    step_back(&path);
}

// insert file in execution queue
void insert_file(char *name, int sockfd, int size) {
    struct file *myfile = new struct file;

    myfile->name = new char[strlen(name)+1];
    strcpy(myfile->name, name);

    myfile->sockfd = sockfd;
    myfile->size = size;

    myqueue->push(myfile);
    pthread_cond_signal(&cond_nonempty);
}


// get the size of the file
int get_filesize(char *path) {
    struct stat pstat;
    if ( stat(path , &pstat) < 0 ) {
        perror("get_filesize: stat");
        exit(1);
    }
    return pstat.st_size;
}


// basically do what "cd .." does / remove a word from the end of a path
void step_back(char **path) {
    char first[strlen(*path)];
    strcpy(first, *path);
    char *substr = strtok( first, "/" );
    if ( strcmp(substr, *path) == 0 ) return;
    char *last;
    while ( substr != NULL ) {
        last = substr;
        substr = strtok(NULL, "/");
    }
    int shift = strlen(*path)-strlen(last)-1;
    char *newpath = *path + shift;
    memmove(newpath, *path, strlen(*path)-strlen(last)-1);
    *path = newpath;
}