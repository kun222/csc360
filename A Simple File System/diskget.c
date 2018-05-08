/*
Assignment 3
diskget
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
#define last_cluster 0xFFF
#define  root_directory 19

/* this function checks the file*/
int check_file(char* ad, char* n, int index){
     ad =ad +s_size*root_directory; 
     
     while(TRUE){
        char* name= malloc(sizeof(char));
        char* ex=malloc(sizeof(char));
        int i;
        
        if((0x0A&ad[11]) !=0 ){
            ad = ad+32;
            continue;
        }
        for(i=0;i<8;i++){
            if(ad[i]==' '){
                break;
            }
            name[i]=ad[i];
        } 
        strcat(name,".");
        for(i=0;i<3;i++){
           ex[i]=ad[8+i];
        } 
        strcat(name,ex);

        if(strcmp(name,n)==0){
            if(index ==1){
                int size=size=((0xFF&ad[31])<<24)+((0xFF&ad[30])<<16)+((0xFF&ad[29])<<8)+(0xFF&ad[28]);
                free(name);
                free(ex);
                return size;
            }else if (index ==2){
                free(name);
                free(ex);
                return (ad[27]<<8)+ad[26];
            }else{
                free(name);
                free(ex);
                printf("error on check_file\n");
                exit(1);
            }
        } 
        ad = ad+32;
        if(ad[0]==0x00){
            free(name);
            free(ex);
            break;
        }
    }
    return -1;
}

int update_add(int f_e){
    int output;
    output = s_size * (31 +f_e);
    return output;
}

int c_fat_e(char*ad, int i){
	
    int index=s_size+((3*i)/2);
	if ((i % 2) != 0) {
        return((0xFF&ad[index+1])<<4)+((0xF0&ad[index])>>4);
	} else {
	    return(0xFF&ad[index])+((0x0F&ad[index+1])<<8);
	}
	
}


/*copying a file from the file system to the current directory*/
void processing(char* n, char* ad, char* n_ad){
    int l_s;
    int fat_entry;
    int physcial_s_a;
    int f_s;
    int r;
    fat_entry=l_s =check_file(ad,n,2);
    physcial_s_a=update_add(fat_entry);
    r=f_s=check_file(ad,n,1);
    
    
	while(TRUE){
        fat_entry=(r==f_s) ? l_s : c_fat_e(ad,fat_entry);
		physcial_s_a=update_add(fat_entry);

		int i=0;
		while(TRUE) {
            
            if (r == 0 || i>=s_size) {
				break;
			}
			n_ad[f_s - r] = ad[i + physcial_s_a];
            i++;
            r--;     
        }
    
        if(c_fat_e(ad,fat_entry) == last_cluster){
            break;
        }
    }
}

/*************************************************************
*     The main function take the user input                  *
*     copies a file from the file system to                  * 
*     the current directory                                  *
*     (file name and extension are case senstive)            *
*************************************************************/
int main(int argc,char* argv[]){
    int disk;
    struct stat set;
    char * address;
    int f_s;
    if(argc<3){
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
  
    f_s=check_file(address,argv[2],1);
    if (f_s<=0){
        munmap(address,set.st_size);
        close(disk);
        printf("file doesn't exist or the file is an empty file\n");
        return 1;
    }    
        
    int new;
    int seek;
    int w;
    char* n_address;
    
   
    new = open(argv[2], O_RDWR | O_CREAT, 0666);
    seek = lseek(new, f_s-1, SEEK_SET);
    w = write(new, "", 1);

    if((new<0) | (seek== -1) | (w !=1) ){
        munmap(address,set.st_size);
        close(disk);
        printf("The file has problem\n");
        return 1;
    }

    n_address =mmap(NULL,f_s,PROT_WRITE, MAP_SHARED, new, 0);
    if (n_address == MAP_FAILED) {
		printf("error on mmap\n");
		return 1;
	}
        
    processing(argv[2],address,n_address);
    munmap(n_address,f_s);
    close(new);
    munmap(address,set.st_size);
    close(disk);
    return 0;
}