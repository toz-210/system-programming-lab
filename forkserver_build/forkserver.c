// forkserver.c

#define FORKSRV_FD 198
#define READ_FD  0
#define WRITE_FD 1
#include<stddef.h>//NULL
#include<unistd.h>//execv,write,read
#include<sys/wait.h> //waitpid
#include<stdlib.h>//exit()
int main() {
    int pipe_fd[2]; // fuzzer↔forkserver
    
    // 1. hello to fuzzer
    write(FORKSRV_FD+1, "\x00\x00\x00\x00",4);

    
    while(1) {
        // 2. forkserver command receive
	int dummy;//buffer to receive
        read(FORKSRV_FD, &dummy,4);//forkserver command receive
        
        // 3. fork
        pid_t pid = fork();
        
        if (pid == 0) {
            // child: target excute
            char *args[] = {"/bin/ls", NULL};
	    execv(args[0], args);
	    exit(1);//when execv fail:

        } else {
            // PID send
            write(FORKSRV_FD+1,&pid,4);
            //waitpid
            int status;
            waitpid(pid, &status, 0);//wait for child and save status
            
            // send status 
            write(FORKSRV_FD+1, &status, 4);
        }
    }
}
