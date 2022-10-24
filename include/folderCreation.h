#include  <iostream>
#include  <cstring>
#include  <unistd.h>
#include  <sys/stat.h>
#include  <fcntl.h>
#include  <sys/types.h>

using namespace std;

char *adjust_dirname(char *);

void adjust_filename(char **, char *);

void create_dir(char *);

int create_file(char *);

void create_path(char *);

void paste_contents(char *, char *);

