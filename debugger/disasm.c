#include<stdio.h>
#include<intypes,h>

#include<capstone/capstone.h>

#define CODE "\x55\x48\x8b\x05\xb8\x13\x00\x00"

int main(void){
	csh handle;
	cs_insn *insn;
	size_t count;

	if(cs_open(CS_ARCH_X86,CS_MODE_64,&handle)!=CS_ERR_OK)
		return -1;

	count=disasm(handle,CODE,sizeof(CODE)-1,0x1000,&insn);

	printf("0x%"PRIx64":%s\t%s",insn[0].address,insn[0].mnemonic,insn[0].op_str);

	cs_free(insn, count)


	

	
