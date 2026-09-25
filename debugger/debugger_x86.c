#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/type.h>//for proccess id
#include<sys/ptrace.h>//essential
#include<sys/user.h>//for register
#include<string.h>//for string
#include<stint.h>//for address
#include<stdbool.h>//for bool
#include<stdlib.h>//for 0x


typedef  struct{
	char *name
	void (*handler)(char *arg,pid_t pid);
}command_t;

typedef struct list{
	
	uintptr_t address;
	long bu_instr;
	bool active;
	struct list *next;
	
}bp_list;//about list struct



void run_cmd(char *address,pid_t pid,*bp_list head){//fork,traceme,exec,breakpoint active,execute until hit the breakpoint

}




*bp_list break_cmd(char *address,pid_t pid,*bp_list head){
	unsigned long addr = strtout(address,NULL,0);//transfer string to addresss
	bp_list *p=malloc(sizeof(bp_list));
	bp_list *current;//for saving current address
	if(head==NULL){//if bp_list head doesn't exist
		head=p;
	}
	else{//if bp_list head exist,go to end of list and chain the current list
	current=head;
	while(current->next!=NULL){
		current=current->next;
	}
	current->next=p;//chain list to tail
	}
	p->address=addr;
	p->bu_instr=ptrace(PTRACE_SEEKDATA,pid,addr,NULL);
	p->active=1;
        p->next=NULL;
	ptrace(PTRACE_POKEDATA,pid,addr,int3);//insert intel x86 stop instruction
	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS,pid,NULL,&regs);
	regs.rip-=1;
	ptrace(PTRACE_SETREGS,pid,NULL,&regs);//rip must be minus one  because int3 trigger RIP plus one for excuting next instruction so we need to decrease
	return head;//return bp_list head 	
}


*bp_list cont_cmd(char *address,pid_t pid,*bp_list head){
	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS,pid,NULL,&regs);
	unsigned long current=regs.rip;//current is register rip address
	ptrace(PTRACE_POKEDATA,pid,current,head->bu_instr);//insert backup instruction into current address
	ptrace(PTRACE_CONT,pid,NULL,NULL);//continue debugger
	return head//return bp_list head
 
}

void info_cmd(char string[],pid_t pid,*bp_list head){

	struct user_regs_struct regs;
	if(strcmp("breakpoint",string)==0){
			}
	else if(strcmp(""))
	{}

	//print total number of breakpoint and current breakpoint addres

}


command_t table[]={
        {"run",run_cmd},
        {"continue",cont_cmd},
        {"nexti",nexti_cmd},
        {"stepi",stepi_cmd},
        {"finish",fin_cmd},//for execute
        {"entry",ent_cmd},
        {"staff",staff_cmd},//for excute&break
        {"break",break_cmd},
        {"info",info_cmd}
}//command table


int main(int argc,char *argv[]){
	
	int status;//for process status
	char buf[128];//it is buffer
	bp_list head=NULL;//for saving bp_lish head pointer and initalize head

	while(1){
	printf("mygdb)");
	
	if(argc<3){
		printf("wrong command\n");
		return 1;
	}//to small parameter
	
	
	pid_t pid=forK();

	if(pid==0){//child process 
		ptrace(PTRACE_TRACEME,NULL,NULL);
		execv(argv[1],&argv[1]);//runtarget	
				}
	else{//parent process(debugger)
	     waitpid(pid,&status);
	     if(WIFSTOPPED(status)){//insert command
		             fgets(buf,sizeof(buf),stdin);//command from keybo
			     char *cmd=strtok(buf," ");//seperate command
			     char *arg=strtok(NULL," ");//seperate parameter
			     int n = sizeof(table)/sizeof(table[0]);
			     for(int i=0;i<n; i++){
				     if(strcmp(cmd,table[i].name)==0){
					     head=table[i].handler(arg,pid,head);//parameter bp_list head
					     break;
				     }

			     }
			     printf("breakpoint hit\n");
			     }
	     if(WIFEXITED(status)){//debugger finished
		     printf("target exit\n");
	     }
