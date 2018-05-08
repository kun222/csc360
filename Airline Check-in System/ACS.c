/* 
airline check-in system 
Kun Ye
v00871367
*/
/*******Includes header files *******/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
/* Substitutes a preprocessor macro */
#define TRUE 1
#define FALSE 0
#define size_queue 300
#define size_input 2000
#define check 5
#define clerk_state 4
#define cal 10
/*create a data structure for customer*/
typedef struct customer
{
    int customer_id;
    double arrival_time;
    double service_time;
} customer;

/*global variables*/
struct timeval init_time;
customer *queue[4][size_queue];
customer customer_list[size_input];

double overall_waiting_time; //overall waiting time for all customers
int length[] = {0, 0, 0, 0}; // store the length of queues
pthread_cond_t Convar;
pthread_mutex_t Mutex[11];
pthread_t customer_thread[size_queue];
/*construct the functions of for queues*/
int serving[3];


/*************** queues's fuctions *************** 
********************************************/

/* insert a customer to a given queue with given queue index i */
void insert(customer *new_customer, int i)
{

    queue[i][length[i]] = new_customer;
    length[i] += 1;
}
/*remove the first customer from the queue wiht given queue index i*/ 
void removed(int i)
{
    int x = 0;
    while (x < (length[i] - 1))
    {
        queue[i][x] = queue[i][x + 1];
        x += 1;
    }
    
}


/*get the customer info and store into the customer object */
void store_info(char customers[size_input][size_input], int cnt)
{

    int i = 1;
    while (i <= cnt)
    {
        if (customers[i][1] == ':')
        {
            customers[i][1] = ',';
        }

        int f = 0;
        int buffer[3];
        char *token = strtok(customers[i], ",");

        while (f < 3 && token != NULL)
        {
            buffer[f] = atoi(token);
            token = strtok(NULL, ",");
            f++;
        }
        
        if(buffer[0]<0||buffer[1]<0||buffer[2]<0){
           printf("input file has negative value");
           exit(1); 
        }
        
        
        
        customer new_customer;
        new_customer.customer_id=buffer[0];
        new_customer.arrival_time=buffer[1];
        new_customer.service_time=buffer[2];
        
        /*
        customer new_customer = {
            buffer[0],
            buffer[1],
            buffer[2]};
        */
        customer_list[i - 1] = new_customer;

        i++;
    }
}


/* find the shortest queue's length from queues 
   it will return the first shortest queues's legnth it found 
   if more than one queue's lengths are the shortest legnth */
int shortest() {
	int shortest = 0;
	int i;
	for (i = 1; i < 4; i++) {
		if (length[i] < length[shortest]) shortest = i;
	}
	return shortest;
}

/* find the longest queue's length from queues 
   it will return the first longest queues's legnth it found 
   if more than one queue's lengths are the longest legnth */

int longest() {
	int longest = 0;
	int i;
	for (i = 1; i < 4; i++) {
		if (length[i] > length[longest]) longest = i;
	}
	return longest;
}


/* it will return the current simulation time*/
double caculate_time()
{
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    long inital = (init_time.tv_sec * 1000000) + init_time.tv_usec;
    long current = (current_time.tv_sec * 1000000) + current_time.tv_usec;
    double difference = (current - inital) / 1000000;
    return difference;
}

/************************************************
*    This is the customer thread function         * 
*    it stores the customer into the queue        *
*    check the customer can be served now         *
*    if yes, let the avaliable clerk serve the    *
*    customer. If no,customers wait in the queue. *
*    After the clerk served the customer it will  *
*    be avaliable to serve the next customer who  *
     is in the longest queue and first position   *                                         *
*************************************************/

void *processing(void *info)
{
    int clerk_id =0;
    int passed=0;// shows if the customer can be served or not
    customer *customer_info = (customer *)info;
   
    usleep(customer_info->arrival_time * 100000);
    printf("A customer arrives customer ID %2d. \n", customer_info->customer_id);
    double arrive_time= caculate_time();
    int index = shortest();
    if (index == 5)
    {
        printf("the length of queue is wrong\n");
        exit(1);
    }

    pthread_mutex_lock(&Mutex[index]);
    printf("A customer enters a queue: the queue ID %1d, and length of the queue %2d. \n", (index + 1), length[index]);
    insert(customer_info, index);
    pthread_mutex_unlock(&Mutex[index]);
    
    if (pthread_mutex_lock(&Mutex[check])!=0){
        printf("Mutex lock has problem\n");
        exit(1);
    }
   
    /* check if the customer can be served right now */
    while (TRUE)
    {
    	passed=0;
        if (serving[1] == 0)
        {
           
            while ( index != longest() || queue[index][0]->customer_id != customer_info->customer_id)
            {
                if (pthread_cond_wait(&Convar, &Mutex[5]) != 0)
                {
                    printf("Error: failed to wait for convar\n");
                    exit(1);
                }
             
                passed =1;
                break; 
            }
            
            if (passed==1){
            	continue;
            }
             
            clerk_id=1;
            break;
        }
        if(serving[2] == 0)
        {
           
            while ( index != longest() || queue[index][0]->customer_id != customer_info->customer_id)
            {
                if (pthread_cond_wait(&Convar, &Mutex[5]) != 0)
                {
                    printf("Error: failed to wait for convar\n");
                    exit(1);
                }
           
                passed=1;
                break;
            }
            
            if (passed==1){
                continue;
            }
             
            clerk_id=2;
            break;
        }
      
   
    }
   
    if(pthread_mutex_unlock(&Mutex[check])!=0){
        printf("Mutex unlock has problem\n");
        exit(1);     
    }
    
    // change the clerk state   
    pthread_mutex_lock(&Mutex[clerk_state]);
    serving[clerk_id] =1;
    pthread_mutex_unlock(&Mutex[clerk_state]);
    
    printf("A clerk starts serving a customer: start time %.2f, the customer ID %2d, the clerk ID %1d. \n", caculate_time(), customer_info->customer_id,clerk_id);
    pthread_mutex_lock(&Mutex[cal]);
    overall_waiting_time +=(caculate_time()-arrive_time);
    length[index] -=1;
    pthread_mutex_unlock(&Mutex[cal]);
    
    usleep(customer_info->service_time * 100000);
    printf("A clerk finishes serving a customer: end time %.2f, the customer ID %2d, the clerk ID %1d . \n", caculate_time(), customer_info->customer_id,clerk_id);
    // change the clerk state
    pthread_mutex_lock(&Mutex[clerk_state]);
    serving[clerk_id] =0;
    pthread_mutex_unlock(&Mutex[clerk_state]);
  
    if (pthread_mutex_lock(&Mutex[index+6]) != 0)
    {
        printf("Mutex lock has problem\n");
        exit(1);
    }

    if (pthread_cond_broadcast(&Convar) != 0)
    {
        printf("Convar has problme\n");
        exit(1);
    }

    removed(index);

    if (pthread_mutex_unlock(&Mutex[index+6]) != 0)
    {
        printf("Mutex unlock has problem\n");
        exit(1);
    }

    pthread_exit(NULL);
}

/*************************************************************
*     The main function take the user input, and get the     *
*     customer information, and it stores the information    *
*     into the customer object. It calls the processing      *
*     thread to do the simulation. It joins thoses thread.   *                                                    
*     In the end it exit thread                                                     *
*************************************************************/

int main(int argc, char *argv[])
{
    int i;
    // initialize attr
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    //initialize Convar
    if (pthread_cond_init(&Convar, NULL) != 0)
        {
            printf("Convar initializing has problem\n");
            exit(1);
        }
    //initialize Mutex
    for(i=0;i<=9;i++){
        if (pthread_mutex_init(&Mutex[i], NULL) != 0)
        {
            printf("mutex initializing has problem\n");
            exit(1);
        }
    }
   
    if (argc < 2)
    {
        printf("error\n");
    }

    char input_file[size_input][size_input];
    int index = 0;
    FILE *fp = fopen(argv[1], "r");

    if (fp == NULL)
    {
        printf("input file doesn't exit\n");
        return 1;
    }
    else
    {
        while (fgets(input_file[index], size_input, fp))
        {
            index += 1;
        }

        fclose(fp);
    }
    // get the number of customers
    int num_customers = atoi(input_file[0]);
    if (num_customers<0){
        printf("number of customer is negative\n");
        return 1;
    }
    
    if (num_customers==0){
        printf("no customers\n");
        return 1;
    }
    store_info(input_file, num_customers);

    // setup the inital time of simulation
    gettimeofday(&init_time, NULL);
    
    // Create customer thread

    for (i = 0; i < num_customers; i++)
    {
        if (pthread_create(&customer_thread[i], &attr, processing, (void *)&customer_list[i]) != 0)
        {
            printf("create customer thread doesn't success\n");
            exit(1);
        }
    }
    // join threads
    int j;
    for (j = 0; j < num_customers; j++)
    {
        if (pthread_join(customer_thread[j], NULL) != 0)
        {
            printf("join customer doesn't thread\n");
            exit(1);
        }
    }
    // destroy attr
    if (pthread_attr_destroy(&attr) != 0)
    {

        printf("destroy attr doesn't success\n");
        exit(1);
    }
    // destory convar
    if (pthread_cond_destroy(&Convar) != 0)
    {
        printf("destroy Convar doesn't success\n");
        exit(1);
    }
    
    // destory all the Mutexs
    for(i=0;i<=9;i++){
        if (pthread_mutex_destroy(&Mutex[i]) != 0)
        {
            printf("destroy Mutex doesn't success\n");
            exit(1);
        }
    }                                             
  
    printf("Average customer waiting time %.2f\n",(overall_waiting_time/num_customers) );
    pthread_exit(NULL);

    return 0;
}
