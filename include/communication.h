#include  <iostream>
#include  <cstring>
#include  <unistd.h>
#include  <pthread.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <dirent.h>
#include  "execQueue.h"

using namespace std;

void *communicate(void *);

char *read_dirname(int);

void content(char *, int, int &);

void insert_file(char *, int, int);

int get_filesize(char *);

void step_back(char **);