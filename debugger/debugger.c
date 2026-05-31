#include<sys/ptrace.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<capstone/capstone.h>
#include<stdint.h>//for unsigned char define

/*fork create parent child process. fork return to parent child PID
 * and return to child zero*/
int main()
{
char cmd[64];//cmd input
 
pid_t pid=fork();
unsigned long addr;//memory address input
unsigned long bu;//backup the value from breaking point
unsigned long bp_addr;//because fo bu address can be contaminated by scanf addr

if(pid==0)//child process
{
	char *args[]={"/usr/bin/ls",NULL};
	ptrace(PTRACE_TRACEME,0,NULL,NULL); //child allow parent to chase himself
	execv("/usr/bin/ls",args);
	

}
else//parent process
{
	wait(NULL);//if child pause, parent awake
	printf("child pid:%d\n",pid);
	while(1){
	scanf("%s %lx",cmd,&addr);

	if(strcmp(cmd,"mem")==0){


	
	        printf("addr:%lx valule:%lx\n",addr,ptrace(PTRACE_PEEKDATA,pid,addr,NULL));	//memory read& printf memory value
	}

	else if(strcmp(cmd,"bp")==0){

		bu=ptrace(PTRACE_PEEKDATA,pid,addr,NULL);//backup the valude into bu 
		long new_val=(bu&~0xFFFFFFFFL)|0xD4200000L;//mask operation beacuse upper 4byte keep but lower 4byte change to D420000L (it is stop byte command. ARM using 4byte instead of intel using 1byte to create bp)
		ptrace(PTRACE_POKEDATA,pid,addr,new_val);//put bp into input address
		//breaking point set
		//I am not using x86 instead I am using ARM
		//so this command is not comptaible to intelx86 
		bp_addr=addr;//because of comtaminated by scanf address when you use cont
	}
	else if(strcmp(cmd,"cont")==0){
		int status;//for distinguish bp hit or child exited
		ptrace(PTRACE_CONT,pid,NULL,NULL);
   		wait(&status);//parent can awake because of child exit or bp hit. we can't distinguish bp hit or child so wait()function can distinguish status (bp or child exit) 
		if(WIFSTOPPED(status)){

		printf("breakpoint hit\n");
		ptrace(PTRACE_POKEDATA,pid,bp_addr,bu);
		}else{
			printf("child exited\n");
			break;
		}
		//continue
	}
	else if(strcmp(cmd,"disasm")==0){
	uint8_t bytes[8];
	csh handle;
	cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &handle);//initialize capstone handle
	long val = ptrace(PTRACE_PEEKDATA,pid,addr,NULL);
	memcpy(bytes, &val, 8);//memorycopy;from val to bytes because cd_sdisasm require uint8_t*
	cs_insn *insn;		       //spend bytes to cs_disasm
	size_t count;
	count = cs_disasm(handle, bytes, sizeof(bytes), addr, 0, &insn);//result store in insn and cs_disasm return count
	for(size_t i=0;i<count; i++){
		printf("0x%lx: %s %s\n", insn[i].address, insn[i].mnemonic, insn[i].op_str);
	}//printf(insn address,insn  mnemonic name,insn operan)
	
	cs_free(insn,count);//because insn,count are malloc()
	

	}
	else if(strcmp(cmd,"exit")==0){
   	 ptrace(PTRACE_KILL, pid, NULL, NULL);
   	 break;//exit command:kill child process and break loop
	}
	else
	{
		printf("wrong command\n");
		ptrace(PTRACE_KILL, pid, NULL, NULL);
		break;
	}
		


}	
}
}

	
