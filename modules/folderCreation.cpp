#include  "folderCreation.h"

char *adjust_dirname(char *name) {
    char temp[strlen(name)+1];
    strcpy(temp, name);

    char *substr = strtok( temp, "/" );

    if ( strcmp(substr, name) == 0 ) return name;
    char *last = new char[strlen(name)];
    while ( substr != NULL ) {
        strcpy(last, substr);
        substr = strtok(NULL, "/");
    }
    return last;
}

void adjust_filename(char **filename, char *dirname) {
    char *fixed = adjust_dirname(dirname);

    char temp[strlen(*filename)+1];
    strcpy(temp, *filename);
    char *substr = strtok( temp, "/" );
    int sum = 0;
    while ( substr != NULL ) {
        if ( strcmp(fixed, substr) == 0 ) break;
        sum += strlen(substr) + 1;
        substr = strtok(NULL, "/");
    }
    memmove(*filename, *filename+sum, strlen(*filename+sum) + 1);
}


// creates directory
void create_dir(char *dirname) {
    if ( mkdir(dirname, 0777) < 0 ) {
        perror("mkdir");
        exit(1);
    }
}


// creates file
int create_file(char *filename) {   // filename is relative path
    int fd;
    if ( (fd = creat(filename, 0777)) < 0 ) {
        perror("creat");
        exit(1);
    }
    return fd;
}


void create_path(char *path) {
    char temp[strlen(path)+1];
    strcpy(temp, path);
    char *substr = strtok( temp, "/" );

    char *subpath = new char[strlen(path)];
    while ( substr != NULL ) {
        strcat(subpath, substr);

        if ( strcmp(subpath, path) == 0 ) {    // is regular file
            if ( access(path, F_OK) == 0 )   // file exists
                remove(path);
            create_file(path);
        } else {
            struct stat pstat;
            if ( stat(subpath, &pstat) < 0 )
                if ( errno == ENOENT )    // directory doesn't exist
                    create_dir(subpath);
        }
        strcat(subpath, "/");
        substr = strtok(NULL, "/");
    }
    delete subpath;
}


void paste_contents(char *path, char *contents) {
    int fd;
    if ( (fd = open(path, O_WRONLY)) < 0 ) {
        perror("open");
        exit(1);
    }
    if ( write(fd, contents, strlen(contents)) < 0 ) {
        perror("write");
        exit(1);
    }
    if ( close(fd) < 0 ) {
        perror("close");
        exit(1);
    }
}