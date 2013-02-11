#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
static jmp_buf env;

void sig_hldr(int signal, siginfo_t *si, void *arg)
{
    printf("Caught segfault at address %p\n", si->si_addr);
    siglongjmp(env,1);
    return;
}

void fun3()
{   
    int *foo = NULL;
    printf("\n %s called.....",__FUNCTION__);
    *foo = 1;
    //raise(11);
    return;
}
void fun2()
{
    printf("\n %s called.....",__FUNCTION__);
    fun3();
    return;
}
void fun1()
{
    printf("\n %s called.....",__FUNCTION__);
    fun2();
    return;
}
int main(void)
{
    int *foo = NULL;
    struct sigaction sa;
    int opt;
    char buf[0];
    setvbuf(stdout, buf, _IOFBF, 0);
    
    memset(&sa, 0, sizeof(sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sig_hldr;
    sa.sa_flags   = SA_SIGINFO;

    sigaction(SIGSEGV, &sa, NULL);
    sigaction(6, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
local:
    if(sigsetjmp(env,1) == 0) 
    {
        printf("\n bookmark set");
        fun1();
    }else {
        printf("\n bookmark retrieved \n Do you want to contnue [yes = 1|No = 0]");
        scanf("%d",&opt);
        if(!opt)
        {
            exit(0);
        }
        goto local;
        
    }

    return 0;
}
