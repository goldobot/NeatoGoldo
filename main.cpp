#include <QApplication>
#include "C_Main.h"

#include <pthread.h>
#include <stdio.h>

pthread_barrier_t g_start_barrier;
pthread_barrier_t g_stop_barrier;

pthread_t g_slave_thread1;
pthread_t g_slave_thread2;
pthread_t g_slave_thread3;

extern bool g_coreslam_ready;

extern void *g_slave_proc1(void*);
extern void *g_slave_proc2(void*);
extern void *g_slave_proc3(void*);

int main(int argc, char *argv[])
{
    int ret;

    g_coreslam_ready = false;

    pthread_barrier_init(&g_start_barrier, NULL, /*N_WORKER_THREADS=*/4);
    pthread_barrier_init(&g_stop_barrier, NULL, /*N_WORKER_THREADS=*/4);

    ret=pthread_create(&g_slave_thread1, NULL, &g_slave_proc1, NULL);
    if(ret!=0) {
        printf("Unable to create slave thread 1\n");
    }

    ret=pthread_create(&g_slave_thread2, NULL, &g_slave_proc2, NULL);
    if(ret!=0) {
        printf("Unable to create slave thread 2\n");
    }

    ret=pthread_create(&g_slave_thread3, NULL, &g_slave_proc3, NULL);
    if(ret!=0) {
        printf("Unable to create slave thread 3\n");
    }

    QApplication app(argc, argv);

    // Main window of the application
    C_Main appMainWin;

    return app.exec();
}
