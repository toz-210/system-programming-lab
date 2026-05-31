// fuzzer.c

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define FORKSRV_FD 198

int main() {
    int ctl_pipe[2];  // fuzzer → forkserver
    int st_pipe[2];   // forkserver → fuzzer
    
    // 1. pipe two create
    pipe(ctl_pipe);
    pipe(st_pipe);
    
    pid_t fs_pid = fork();
    
    if (fs_pid == 0) {
        // forkserver process
        // 2. ctl_pipe read end to  198
        dup2(ctl_pipe[0], FORKSRV_FD);
        
        // 3. st_pipe wirte end to  199
        dup2(st_pipe[1], FORKSRV_FD+1);
        
	 char *args[]={"./forkserver",NULL};
	execv("./forkserver",args);//excute forkserver
        
    } else {
        // fuzzer process
        // 5. hello receive (forkserver ready confirm)
	int dummy;
        read(st_pipe[0],&dummy,4 );
        
        // 6.command send
	int command;
        write(ctl_pipe[1],&command,4);
        
        // 7. PID receive
	pid_t pid;
        read(st_pipe[0],&pid,4);
        
        // 8. status receive
	int status;
        read(st_pipe[0],&status,4);
    }
}
