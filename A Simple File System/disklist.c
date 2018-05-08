/*
Assignment 3
disklist
Kun Ye
*/
/*******Includes header files *******/
#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
/* Substitutes a preprocessor macro */
#define FALSE 0
#define TRUE 1
#define s_size 512



/* this function display the information in the root directory*/
void display_root_directory(char* ad){
    while(TRUE){
       
        char attributes;
        int size;
        char* name= malloc(sizeof(char));
        char* ex=malloc(sizeof(char));
        
        if((ad[11]& 0x10) !=0x10){
            attributes='F';
        }else{
            attributes='D';
        }
        
        size=((0xFF&ad[31])<<24)+((0xFF&ad[30])<<16)+((0xFF&ad[29])<<8)+(0xFF&ad[28]);
        int i;
        for(i=0;i<8;i++){
            if(ad[i] == ' '){
                break;
            }
            name[i]=ad[i];
        } 
        strcat(name,".");
        for(i = 0;i < 3; i++){
           ex[i]=ad[i+8];
        } 
        strcat(name,ex);
        
        int h=(0xF8&ad[15])>>3;
        int min=((0x07&ad[15])<<3)+((0xE0&ad[14])>>5);
        int y=1980+((0xFE&ad[17])>>1);
        int m=((0x01&ad[17])<<3)+((0xE0&ad[16])>>5);
        int d=(0x1F&ad[16]);
       
        if((0x0A&ad[11]) ==0){
            printf("%c %10d %20s %d-%02d-%02d %02d:%02d\n",attributes,size,name,y,m,d,h,min);
        }
        ad = ad+32;
        if(ad[0]==0x00){
            free(name);
            free(ex);
            break;
        }
    }
}

/*************************************************************
*     The main function take the user input                  *
*     Displays the contents of the root directory            *
*     in the file system                                     *
*************************************************************/

int main(int argc,char* argv[]){
    int disk;
    struct stat set;
    char * address;
    if(argc<2){
        printf("the input format is incorrect\n");
        return 1;
    }
     
    disk =open(argv[1],O_RDWR);//  Open the file so that it can be read from and written to.
    if(disk<0){
        printf("couldn't read the disk image\n");
        return 1;
    }
     
    
    fstat(disk,&set);
    /* creates a new mapping in the virtual address 
    space of the  calling process*/
    address=mmap(NULL,set.st_size,PROT_READ,MAP_SHARED,disk,0);
    if(address== MAP_FAILED){
        printf("error on mmap\n");
        return 1;
    }
    address =s_size*19 + address;
    
    display_root_directory(address);
    /*remove any mappings for those entire pages 
    containing any part of the address space*/
    munmap(address,set.st_size);
    close(disk);
    return 0;
}
