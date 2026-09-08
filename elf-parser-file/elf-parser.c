#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>

int main(int argc, char *argv[]) {
    // 1. 파일 열기
    FILE *fp;
    int found;
    int symtable_entry_num;
    fp = fopen(argv[1],"rb");
if(fp==NULL)
{
printf("failed to open file\n");
return 1;
}	

    // 2. ELF Header 읽기 (Elf64_Ehdr)
    Elf64_Ehdr ehdr;
    fread(&ehdr, sizeof(ehdr),1,fp);
    // 3. magic number 검사
    //    ehdr.e_ident[0~3] == 0x7f 'E' 'L' 'F'
    if(ehdr.e_ident[0]!=0x7f || ehdr.e_ident[1]!='E'||ehdr.e_ident[2]!='L'||ehdr.e_ident[3]!='F'){
        printf("This is not anELF file.\n");
        return 1;
    }

//
    // 4. Program Header Table 읽기
    
    //    fseek → e_phoff 위치로
    fseek(fp, ehdr.e_phoff, SEEK_SET);
   
    //    read → Elf64_Phdr 배열
     Elf64_Phdr phdr[ehdr.e_phnum];
    fread(phdr, sizeof(Elf64_Phdr), ehdr.e_phnum, fp);


    // 5. Section Header Table read & shstrtab read
    //    fseek → e_shoff 위치로
    //    read → Elf64_Shdr 배열
    fseek(fp,ehdr.e_shoff, SEEK_SET);
    Elf64_Shdr shdr[ehdr.e_shnum];
    fread(shdr, sizeof(Elf64_Shdr), ehdr.e_shnum, fp);
    fseek(fp,shdr[ehdr.e_shstrndx].sh_offset,SEEK_SET);
    char *shstrtab=malloc(shdr[ehdr.e_shstrndx].sh_size);
    fread(shstrtab, sizeof(char),shdr[ehdr.e_shstrndx].sh_size,fp);

   

    // 6. 출력
    printf("========ELF Header=========\n");

    printf("e_type: %d\n",ehdr.e_type);
    printf("e_entry: 0x%lx\n", ehdr.e_entry);
   printf("e_phnum: %d\n", ehdr.e_phnum);
    printf("e_shnum: %d\n", ehdr.e_shnum);


    
    printf("========Program Headers=========\n");

    for(int i=0; i<ehdr.e_phnum;i++)
	{
    printf("p_type:    %d\n", phdr[i].p_type);
    printf("p_offset:  %lx\n", phdr[i].p_offset);
    printf("p_vaddr:   0x%lx\n", phdr[i].p_vaddr);
    switch(phdr[i].p_type){
        case PT_NULL: printf("NULL\n"); break;
        case PT_LOAD: printf("LOAD\n"); break;
        case PT_DYNAMIC: printf("DYNAMIC\n"); break;
	}
	}

		
    printf("========Section Headers=========\n");

    for(int i=0; i<ehdr.e_shnum;i++)
	{
    printf("sh_name:   %s\n", &shstrtab[shdr[i].sh_name]);
    printf("sh_type:   %d\n", shdr[i].sh_type);
    printf("sh_addr:   %lx\n", shdr[i].sh_addr);
	}



    printf("========Symbols========\n");

   found=0;
	     //( symboltable & strtab read printf)and( symbol name & address)
     for(int i=0; i<ehdr.e_shnum;i++)
        {

         if(shdr[i].sh_type==SHT_SYMTAB)
         {

            fseek(fp,shdr[i].sh_offset,SEEK_SET);
            Elf64_Sym *sym=malloc(shdr[i].sh_size);
            symtable_entry_num=shdr[i].sh_size/shdr[i].sh_entsize;//save symtable_entry num for read and print
            fread(sym,sizeof(Elf64_Sym),symtable_entry_num,fp);//save symtable to sym
            fseek(fp,shdr[shdr[i].sh_link].sh_offset,SEEK_SET);//fseek to offset of strtab section header
            char *strtab=malloc(shdr[shdr[i].sh_link].sh_size);//shdr[shdr[i].sh_link] is section header of strtab
            fread(strtab,sizeof(char),shdr[shdr[i].sh_link].sh_size,fp);//fread char* strtab size
           for(int j=0; j<symtable_entry_num;j++)
	    printf("%s 0x%lx\n",&strtab[sym[j].st_name],sym[j].st_value);//printf
	   found = 1;							       
         }
        }
	
	if(found==0)
	printf("stripped\n");


}

