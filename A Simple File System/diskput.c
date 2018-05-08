/*
Assignment 3
diskput
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
#include <time.h>
#include <unistd.h>
/* Substitutes a preprocessor macro */
#define FALSE 0
#define TRUE 1
#define s_size 512
#define root_directory 19


int c_fat_e(char*ad, int i){
	
	int index=s_size + ((3*i) / 2);
	if ((i % 2) != 0) {
        return((0xFF&ad[index+1]) << 4)+((0xF0&ad[index]) >> 4);
        
	} else {
	    return(0xFF&ad[index]) + ((0x0F&ad[index+1]) << 8);
	}
}

/*set the time for the new file*/
void set_time(char* ad,int m, int f_size){
    time_t s_time = time(NULL);
	struct tm *current_t_info = localtime(&s_time);//a calendar date and time broken down into its components.    

    ad[14] = 0x00;
	ad[15] = 0x00;
	ad[16] = 0x00;
	ad[17] = 0x00;
    
    ad[14] |= ((current_t_info->tm_min)-((0x07&ad[15])<<3))<<5;
    ad[15] |= 0xF8&((current_t_info->tm_hour)<<3);
	ad[15] |= ((current_t_info->tm_min)-((0xE0&ad[14])>>5))>>3;
    ad[16] |= ((current_t_info->tm_mon+1)-((0x01&ad[17])<<3))<<5;
	ad[16] |= (0x1F&(current_t_info->tm_mday));
    ad[17] |= (current_t_info->tm_year-80)<<1;
	ad[17] |= ((current_t_info->tm_mon+1)-((0xE0&ad[16])>>5))>>3;
	
	ad[27] = (m-ad[26])>>8;
	ad[26] = 0xFF&(m-(ad[27]<<8));
	
}


void set_info(char* ad,char* n, int m, int f_size){

	while (TRUE) {
        if(ad[0]==0x00){
            break;
        }
        ad += 32;
	}
	int i;
	int f = -1;
    char ch;
    for (i = 0; i < 8; i++) {
		ch = n[i];
		if (ch == '.') {
			f = i;
		}
		ad[i] = (f == -1) ? ch : ' ';
	}
	for (i = 0; i < 3; i++) {
		ad[i+8] = n[i+f+1];
	}
	ad[11] = 0x00;
    set_time(ad, m, f_size);
    ad[31] = (0xFF000000&f_size) >> 24;
    ad[30] = (0x00FF0000&f_size) >> 16;
    ad[29] = (0x0000FF00&f_size) >> 8;
    ad[28] = (0x000000FF&f_size);
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


int c_next_f(char* ad){
    int i=2;
    while(c_fat_e(ad,i)!=0x00){
        i++;
    }
    return i;
}

void set_e(char* ad, int bytes ,int c ){
    int index=0;
    if ((c % 2) != 0) {
        index = s_size + ((3*c) / 2);
        ad[index + 1] = (bytes >> 4) & 0xFF;
        ad[index] = (bytes << 4) & 0xF0;
		
	} else {
        index = s_size + ((3*c) / 2);
        ad[index + 1] = (bytes >> 8) & 0x0F;
		ad[index] = bytes & 0xFF;
	}
    
}


/*start copying the file */
void cpy(int f_size ,char* n, char* ad,char*n_ad){
    int r =f_size;
    int m =c_next_f(ad +s_size);
    int addr;
    int i;
    int new;
    set_info(ad +s_size * 19, n, m, f_size);
    while(TRUE){
        if(r <= 0){
            break;
        }

        addr =(31+m)*s_size;
        for(i=0;i<s_size;i++){
            ad[i + addr] = n_ad[f_size - r];
            r--;
            if(r==0){
                set_e(ad+s_size,0xFFF,m);
            return;
            }
        }

        set_e(ad+s_size, 0x69, m);
        new = c_next_f(ad+s_size);
        set_e(ad+s_size, new, m);
        m = new;
    }


}


/*************************************************************
*     The main function take the user input                  *
*     copies a file from the current Linux directory into    *
*     the root directory of the file system                  *  
*     (file name and extension are case senstive)            *
*************************************************************/
int main(int argc,char* argv[]){
    int disk;
    struct stat set;
    char * address;
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
    address=mmap(NULL,set.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,disk,0);
    if(address== MAP_FAILED){
        printf("error on mmap\n");
        return 1;
    }

    
    
    int new;
    struct stat n_set;
    char * n_address;

    new =open(argv[2],O_RDWR);//  Open the file so that it can be read from and written to.
    if(new<0){
        printf("there is no such file\n");
        return 1;
    }
     
    fstat(new,&n_set);
    /* creates a new mapping in the virtual address 
    space of the  calling process*/
    n_address=mmap(NULL,n_set.st_size,PROT_READ,MAP_SHARED,new,0);
    if(n_address== MAP_FAILED){
        printf("error on mmap\n");
        return 1;
    }
    

    int total_size=((address[19]+ (address[20]<<8))*s_size);
    int free_size=(c_free_size(address,total_size));

    if( n_set.st_size > free_size){
        printf(" there are no space for this file");
        return 0;
    }
    cpy(n_set.st_size,argv[2],address,n_address);   

    munmap(address,set.st_size);
    close(disk);
    munmap(n_address,n_set.st_size);
    close(new);

    return 0;

}    