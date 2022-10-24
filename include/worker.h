#include  <iostream>
#include  <cstring>
#include  <fcntl.h>
#include  <unistd.h>
#include  <map>
#include  <sys/stat.h>
#include  "execQueue.h"
#include  "mutexDict.h"

using namespace std;

void *work(void *);

void send(int, char *, int);
