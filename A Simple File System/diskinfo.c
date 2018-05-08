/*
Assignment 3
diskinfo
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

/*get the label of disk*/
void get_label(char *ad,char* label){
    int i;
    for(i=0;i<8;i++){
        label[i] = ad[i+43];
    }
    /* if not in the boot sector */
    if(label[0]==' '){
        ad=ad+s_size*19;// go to boot directory
        while(TRUE){
            if(ad[0]==0x00){
                break;
            }
            //*
            if(ad[11]==0x08){
                for(i=0;i<8;i++){
                    label[i] = ad[i];
                }
            }
            ad=ad+32;
        }
  
    }

}

int c_fat_e(char*ad, int i){
    int index=s_size + ((3*i) / 2);
	if ((i % 2) != 0) {
        return((0xFF&ad[index+1]) << 4)+((0xF0&ad[index]) >> 4);
        
	} else {
	    return(0xFF&ad[index]) + ((0x0F&ad[index+1]) << 8);
	}
}

int c_free_size(char *ad, int total){
    int cnt=0;
    int i;
    int num_sectors=total/s_size;
    int output;
    for(i=2; i<=num_sectors-32;i++){
        if(c_fat_e(ad,i)==0x00){
            cnt++;
        }
    }
    output =cnt *s_size;
    return output;

}

int c_num_files_root(char * ad){
     int cnt=0;
   
    while(TRUE){
        if(ad[0]==0x00){
            break;
        }
        if((ad[11] ) != 0 || (ad[11] )!=0 || (ad[11] ) !=0){
            cnt ++;
  
        } 
        ad= ad+32;
    } 

    return cnt;

}

/*************************************************************
*     The main function take the user input                  *
*     Displays basic information about the file system       *                            *
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
    /*initialize all the variables*/
    char *name =malloc(sizeof(char));
    char *label =malloc(sizeof(char)); 
    int total_size=0;
    int free_size=0;
    int num_files_root=0;
    int num_fat_copies=0;
    int sec_per_fat=0;
    int i=0;
    for(i=0;i<8;i++){
        name[i] = address[i+3];

    }
    
    get_label(address,label);
    total_size=(address[20]<<8)*s_size + address[19]*s_size;
    free_size=(c_free_size(address,total_size));
    num_files_root=c_num_files_root(address+s_size*19);
    num_fat_copies=address[16];
    sec_per_fat=(address[23]<<8) + address[22];
    
    printf("OS Name: %s\n", name);
    printf("Label of the disk: %s\n",label);
    printf("Total size of the disk: %d\n", total_size);
    printf("Free size of the disk: %d\n", free_size);
    printf("\n");
    printf("==============\n");
    printf("The number of files in the root directory (not including subdirectories): %d\n", num_files_root);
    printf("\n");
    printf("=============\n");
    printf("Number of FAT copies: %d\n", num_fat_copies);
    printf("Sectors per FAT: %d\n", sec_per_fat);
    free(name);
    free(label);
    
    munmap(address,set.st_size);
    close(disk);
    return 0;
}