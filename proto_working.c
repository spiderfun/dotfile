#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/syscall.h>
static jmp_buf env;
#define N_THR 2
pthread_t tid[N_THR];
static int gloc;

int func3()
{
    int *foo= NULL;
    printf("\n %s called in thread id =%lu \n",__FUNCTION__,pthread_self());
    *foo = 1;
    return 0;
}
int func2()
{
    printf("\n %s called in thread id =%lu \n",__FUNCTION__,pthread_self());
    func3();
    return;
}
int thr_fn1()
{
  
    printf("\n %s called in thread id =%lu \n",__FUNCTION__,pthread_self());
    tid[gloc++] = pthread_self();
    thr_fn();
    return;
}
#if 1
int thr_fn()
{
    int opt,val,loc,i;
    pthread_t id;
#if 0        
    id = pthread_self();
    for (loc = 0; loc < N_THR; loc++)
    {
        if (tid[loc] == id)
        break;
    }
#endif

    printf("\n %s called in thread id =%lu   \n",__FUNCTION__,pthread_self());
    while(1)
    {
local:
    if(sigsetjmp(env,loc+1) == 0) 
    {
        printf("\n bookmark set");
        func2();
    }else {
        printf("\n bookmark retrieved \n Do you want to contnue [yes = 1|No = 0] \nthread id %lu\n",(long)pthread_self());
        scanf("%d",&opt);
        if(!opt)
        {
            exit(0);
        }
        goto local;
        
    }
    }
}
#endif

void sig_hldr(int signal, siginfo_t *si, void *arg)
{
    pthread_t id;
    int i;
    //printf("Caught segfault at address %p  ", si->si_addr);

    id = pthread_self();
    printf("\n thread id = %lu\n",id);
    for (i = 0; i < gloc; i++)
    {
        if (tid[i] == id)
            break;
     }
    if (i >= gloc)
        printf("\nmain thread: caught signal %d\n", signal);
    else
        printf("\nthread no %d: thread id %lu: caught signal %d\n", i+1, id, signal);

    siglongjmp(env, i+1);
    return;
}
main(int argc , char *argv[])
{
    int i;
    void *ret;
    struct sigaction sa;
    int err;
    pthread_t ltid[10];
    printf("\n Demo started.....");
    
    memset(&sa, 0, sizeof(sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sig_hldr;
    sa.sa_flags   = SA_SIGINFO;

    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);

    for (i = 0; i < N_THR; i++) {
        err = pthread_create(&ltid[i], NULL, thr_fn1, NULL);
        if (err != 0)
            printf("\n can't create thread");
           
        //printf("\n thread created id = %lu\n",tid[i]);   
            
    }

    pthread_join(tid[1],err);




}
