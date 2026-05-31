#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>

int main(int argc, char *argv[]) {
    // 1. 파일 열기
    FILE *fp;
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

    // 4. Program Header Table 읽기
    
    //    fseek → e_phoff 위치로
    fseek(fp, ehdr.e_phoff, SEEK_SET);
   ;
    //    read → Elf64_Phdr 배열
     Elf64_Phdr phdr[ehdr.e_phnum];
    fread(phdr, sizeof(Elf64_Phdr), ehdr.e_phnum, fp);
    // 5. Section Header Table 읽기
    //    fseek → e_shoff 위치로
    //    read → Elf64_Shdr 배열
    fseek(fp,ehdr.e_shoff, SEEK_SET);
    Elf64_Shdr shdr[ehdr.e_shnum];
    fread(shdr, sizeof(Elf64_Shdr), ehdr.e_shnum, fp);
    // 6. 출력
    printf("========ELF Header=========\n");

    printf("e_type: %d\n", ehdr.e_type);
    printf("e_entry: 0x%lx\n", ehdr.e_entry);
    printf("e_phnum: %d\n", ehdr.e_phnum);
    printf("e_shnum: %d\n", ehdr.e_shnum);


    
    printf("========Program Headers=========\n");

    for(int i=0; i<ehdr.e_phnum;i++)
	{
    printf("p_type:    %d\n", phdr[i].p_type);
    printf("p_offset:  %lx\n", phdr[i].p_offset);
    printf("p_vaddr:   %lx\n", phdr[i].p_vaddr);
      switch(phdr[i].p_type){
        case PT_NULL: printf("NULL\n"); break;
        case PT_LOAD: printf("LOAD\n"); break;
        case PT_DYNAMIC: printf("DYNAMIC\n"); break;
	}
	}

		
    printf("========Section Headers=========\n");

    for(int i=0; i<ehdr.e_shnum;i++)
	{
    printf("sh_name:   %d\n", shdr[i].sh_name);
    printf("sh_type:   %d\n", shdr[i].sh_type);
    printf("sh_addr:   %lx\n", shdr[i].sh_addr);
	}
}

