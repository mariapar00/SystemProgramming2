#pragma once
#include  <iostream>
#include  <cstring>
#include  <queue>

using namespace std;

struct file {
    char *name;
    int sockfd;
    int size;
};

class ExecQueue {
    private:
        queue<struct file *> equeue;
        int size;
        int max_size;
    public:
        ExecQueue();
        ~ExecQueue();

        void set_maxsize(int);
        int get_size()const;
        int get_maxsize()const;

        struct file *qfront()const;

        void push(struct file *);
        struct file *pop();
};