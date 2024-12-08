#ifndef __MONITOR_H
#define __MONITOR_H
#include<pthread.h>
class Monitor {
public:
    pthread_mutex_t  mut;
    Monitor() {
        pthread_mutex_init(&mut, NULL);
    }
    class Condition {
    public:
        Monitor *owner;
        pthread_cond_t cond;
        Condition(Monitor *o) {
                owner = o;
                pthread_cond_init(&cond, NULL) ; 
        }
        void wait() {  pthread_cond_wait(&cond, &owner->mut);}
        int timedwait(struct timespec *abstime) { return pthread_cond_timedwait(&cond, &owner->mut, abstime); }
        void notify() { pthread_cond_signal(&cond);}
        void notifyAll() { pthread_cond_broadcast(&cond);}
    };
    class Lock {
        Monitor *owner;
    public:
        Lock(Monitor *o) {
            owner = o;
            pthread_mutex_lock(&owner->mut);
        }
        ~Lock() { 
            pthread_mutex_unlock(&owner->mut);
        }
        void lock() { pthread_mutex_lock(&owner->mut);}
        void unlock() { pthread_mutex_unlock(&owner->mut);}
    };
};

#endif

