/* 
simplified version of background execution 
Kun Ye
v00871367
*/

/******* Includes header files *******/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
#include <signal.h>

/* Substitutes a preprocessor macro */
#define TRUE 1
#define FALSE 0
/* global values for check commands   */
char *correct_command[] = {"bg", "bglist", "bgkill", "bgstop", "bgstart", "pstat"};

/* this fucntion takes char * type as input
*  and return true if the input is number. 
*  Otherwise, it returns false. 
*/

int numerical_check(char *input)
{
    int len = strlen(input);
    int i;
    for (i = 0; i < len; i++)
    {
        if (!isdigit(input[i]))
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*************** Linked List *************** 
********************************************/

/*create a linked list note */
typedef struct node
{
    pid_t processID;    // the ID of process(pid)
    char *process_name; // the program's name
    int Running_State;  // the process is running of not
    struct node *next;

} node;
/* create the head for linked list*/
node *head = NULL;

/* add a new node to the start of the list */
void addNode(pid_t processID, char *process_name)
{

    node *new_node;
    new_node = malloc(sizeof(node));
    new_node->processID = processID;
    new_node->process_name = process_name;
    new_node->Running_State = TRUE;
    new_node->next = head;
    head = new_node;
    return;
}

/* retrun a node with given processID */
node *return_node(pid_t processID)
{
    node *current = head;

    while (current != NULL)
    {
        if (current->processID == processID)
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

/* remove a node with given processID */
void remove_node(pid_t processID)
{

    node *current = head;
    node *parentNode = NULL;

    if (head->processID == processID)
    {
        head = head->next;
        free(current);
    }

    while (current != NULL)
    {
        if (current->processID == processID)
        {
            parentNode->next = current->next;
            free(current);
            return;
        }

        parentNode = current;
        current = current->next;
    }

    return;
}
/* cheeck if the node with processID is exist or not in the listed list */
int check(pid_t processID)
{

    node *current = head;

    while (current != NULL)
    {
        if (current->processID == processID)
        {
            return TRUE;
        }

        current = current->next;
    }

    return FALSE;
}

/*****************************************
*    These are the command functions:    * 
*    bg(cmd),bglist(),bgkill(processId).
*    bgstop(processID),bgstart()
*    pstat(processID)                    *
******************************************/

/* creat a process and start running the program in the background */
void bg(char **std_input)
{
    pid_t processID = fork();
    if (processID < 0)
    {
        printf("the creation of the child process was unsuccessful\n");
        exit(1);
    }
    else if (processID == 0) //run child process
    {

        if (execvp(std_input[1], &std_input[1]) < 0)
            perror("Error on execvp");
        printf("Child finished.\n");
        exit(1);
    }
    else
    { //run parenet process
        printf("Started background process %d\n", processID);
        addNode(processID, std_input[1]);
        sleep(1);
    }
}

/*run the bglist displays a list of all the 
programs currently executing in the backgroud*/
void bglist()
{
    int cnt = 0;
    node *current = head;
    while (current != NULL)
    {
        if (current->Running_State)
        {

            printf("%d:  %s\n", current->processID, current->process_name);
            cnt++;
        }

        current = current->next;
    }

    printf("Total background jobs:  %d\n", cnt);
    return;
}

/*run the bgkill() sends a TERM signal to the 
job with process ID to terminate that job*/
void bgkill(pid_t processID)
{

    int retVal = kill(processID, SIGTERM);

    if (retVal < 0)
    {
        printf("the bgkill command is unsuccessful");
        return;
    }
    else
    {
        sleep(1);
        return;
    }
}

/* bgstop pid will send the STOP signal to the 
job pid to stop (temporarily) that job. */
void bgstop(pid_t processID)
{
    int retVal = kill(processID, SIGSTOP);

    if (retVal < 0)
    {
        printf("the bgstop command is unsuccessful");
        return;
    }
    else
    {

        sleep(1);
        return;
    }
}

/* pid will send the CONT signal to the job pid to 
re-start that job (which has been previously stopped). */
void bgstart(pid_t processID)
{
    int retVal = kill(processID, SIGCONT);

    if (retVal < 0)
    {
        printf("the bgstart command is unsuccessful");
        return;
    }
    else
    {

        sleep(1);
        return;
    }
}

/* pstat pid to list the following information related 
to process pid, where pid is the Process ID */
void pstat(pid_t processID)
{
    char stat[180];
    char status[180];
    snprintf(stat, 180, "/proc/%d/stat", processID);
    snprintf(status, 180, "/proc/%d/status", processID);
    char buffer[500];
    char *stat_array[500];

    /* start copying from file stat */

    FILE *fstat;
    fstat = fopen(stat, "r");
    if (fstat == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, sizeof(buffer), fstat) != NULL)
    {
        char *token = strtok(buffer, " ");
        int i = 0;
        while (token != NULL)
        {
            stat_array[i] = token;

            token = strtok(NULL, " ");

            i++;
        }
    }
    fclose(fstat);

    /* print the infromation needed from file stat */
    float u_time = (float)strtoul(stat_array[13], NULL, 0) / sysconf(_SC_CLK_TCK);
    float s_time = (float)strtoul(stat_array[14], NULL, 0) / sysconf(_SC_CLK_TCK);
    printf("comm:  %s\n", stat_array[1]);
    printf("state:  %s\n", stat_array[2]);
    printf("utime:  %f\n", u_time);
    printf("stime:  %f\n", s_time);
    printf("rss:  %s\n", stat_array[23]);

    /* start copying from file status */
    char status_array[180][180];
    FILE *fstatus;
    fstatus = fopen(status, "r");
    if (fstat == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    int s = 0;
    while (fgets(status_array[s], 180, fstatus) != NULL)
    {
        s++;
    }

    fclose(fstatus);

    /* Print the information needed for file status */
    printf("%s", status_array[39]);
    printf("%s", status_array[40]);
}

/* update the process states */
void update()
{
    pid_t processID;
    int status;
    int opts = WNOHANG | WUNTRACED | WCONTINUED;
    while (TRUE)
    {
        processID = waitpid(-1, &status, opts);

        if (processID <= 0)
        {
            break;
        }

        else
        {
            if (WIFEXITED(status))
            {
                /*Evaluates to a non-zero value if status was returned 
                for a child process that terminated normally.*/
                remove_node(processID);
                printf("process %d terminated normally.\n", processID);
            }
            else if (WIFSIGNALED(status))
            {
                /*Evaluates to a non-zero value if status was returned for a child process 
                that terminated due to the receipt of a signal that was not caught*/
                remove_node(processID);
                printf("process %d has terminated with bgkill command.\n", processID);
            }
            else if (WIFSTOPPED(status))
            {
                /*Evaluates to a non-zero value if status was returned 
                for a child process that is currently stopped.*/
                node *process = return_node(processID);
                process->Running_State = FALSE;
                printf("process %d has stopped with bgstop command.\n", processID);
            }
            else if (WIFCONTINUED(status))
            {
                /*Evaluates to a non-zero value if status was returned for 
                a child process that has continued from a job control stop.*/
                node *process = return_node(processID);
                process->Running_State = TRUE;
                printf("process %d has started with bgstart command.\n", processID);
            }
        }
    }
}

/************************************************************
*   Check if the input command belogns to command array     *
*   check if the input is valid or not                      *
*   base on  input determine which function will be called  *                                        *
*************************************************************/
int determine_command(char **std_input)
{

    int index = 0;
    int i;
    for (i = 0; i <= 5; i++)
    {

        if (strcmp(std_input[0], correct_command[i]) == 0)
        {
            break;
        }
        index++;
    }

    /*based on the index number call functions*/

    if (index == 0) // call function bg()
    {
        if (std_input[1] == NULL)
        {
            printf("Error: no process name\n");
            return FALSE;
        }

        bg(std_input);
        return TRUE;
    }
    else if (index == 1) // call function bglist()
    {
        if (std_input[1] != NULL)
        {
            printf("Error: you shouldn't input anything execpt bglist\n");
            return FALSE;
        }

        bglist();
        return TRUE;
    }
    else if (index == 2) //call function bgkill()
    {
        if (std_input[1] == NULL || !numerical_check(std_input[1]))
        {
            printf("Error: you should give a process ID(number)\n");
            return FALSE;
        }

        pid_t processID = atoi(std_input[1]);
        if (!check(processID))
        {
            printf("Error: Process %d does not exist\n", processID);
            return FALSE;
        }
        bgkill(processID);
        return TRUE;
    }
    else if (index == 3) //call function bgstop()
    {
        if (std_input[1] == NULL || !numerical_check(std_input[1]))
        {
            printf("Error: you should give a process ID(number)\n");
            return FALSE;
        }

        pid_t processID = atoi(std_input[1]);
        if (!check(processID))
        {
            printf("Error: Process %d does not exist\n", processID);
            return FALSE;
        }
        bgstop(processID);
        return TRUE;
    }
    else if (index == 4) //call function bgstart()
    {
        if (std_input[1] == NULL || !numerical_check(std_input[1]))
        {
            printf("Error: you should give a process ID(number)\n");
            return FALSE;
        }

        pid_t processID = atoi(std_input[1]);
        if (!check(processID))
        {
            printf("Error: Process %d does not exist\n", processID);
            return FALSE;
        }
        bgstart(processID);
        return TRUE;
    }
    else if (index == 5) //call function pstat()
    {
        if (std_input[1] == NULL || !numerical_check(std_input[1]))
        {
            printf("Error: you should give a process ID(number)\n");
            return FALSE;
        }

        pid_t processID = atoi(std_input[1]);
        if (!check(processID))
        {
            printf("Error: Process %d does not exist\n", processID);
            return FALSE;
        }
        pstat(processID);
        return TRUE;
    }
    else
    {
        printf("%s:  command not found\n", std_input[0]);
        return FALSE;
    }
}

/************************************************************
*     The main function take the user input, and call       *
*     function to determine the input and undate the        *
*     processes state                                       *
*************************************************************/
int main(int argc, char *argv[])
{

    while (TRUE)
    {
        char *std_input[180];
        char *buffer = readline("PMan:  > ");
        if (*buffer == 0)
        {
            return FALSE;
        }
        char *token = strtok(buffer, " ");
        int i = 0;
        while (i < 180)
        {
            std_input[i] = token;

            token = strtok(NULL, " ");

            i++;
        }

        update();
        determine_command(std_input);
        update();
    }

    return 0;
}