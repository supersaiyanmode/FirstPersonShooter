#ifndef THREAD_H
#define THREAD_H
#include <pthread.h>

template <typename T, typename P>
class Thread{
public:
    Thread(T&, void (T::*)(P), P);
    void start();
    static void sleep(int);
    void join();
    void detach();
private:
    T& object;
    void (T::*function)(P);
    P argument;
#ifdef WIN32
    HANDLE    thread;
    static DWORD createThread(void*);
#else
    pthread_t thread;
    static void* createThread(void*);
#endif
};
#endif // THREAD_H
