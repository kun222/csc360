Kun Ye
V00871367
360 assignment1

*******************

"simplified version of background"
PMan allows a program to be started in the background—that is, the program is running, but PMan
continues to accept input from the user. This is a simplified version of background
execution that supports executing processes in the background


*******************

In order to run my code there are two steps:
Step1: excute "make" in the terminal  (if the code it's already been complied, just skip step1 and start from step 2 instead)
Step2: excute "/.PMan" in the terminal


*******************
This program only support 6 command:
"bg" takes (cmd), "bglist", "bgkill" takes processID, "bgstop" takes processID,
"bgstart" takes processID, "pstat" takes processID.
Those will be explained in the section below.
 
*******************
Features of this program

Command:
bg: creat a new process and running in the background

bglist: will have PMan display a list of all the programs currently executing in
the background. (The bglist only displays the process that is running in the backgroud.
if the process is not running in the background,it will not display it. 
you have to use bgstart to start the process again, in order to display this process.)

bgkill: pid will send the TERM signal to the job with process ID pid to terminate
that job. (If the process running state is FALSE, it can not be terminated   
right away. If you already used bgkill on a process that runnng state is FALSE, you have to
use bgstart, after that the process will be terminated immediately).

bgstop: pid will send the STOP signal to the job pid to stop (temporarily) that
job.

bgstart: pid will send the CONT signal to the job pid to re-start that job (which
has been previously stopped).

pstat: pid to list the following information "comm","state","utime","stime","rss","voluntary ctxt switches","nonvoluntary ctxt switches"
related to process pid

*******************
note: you can use "Ctrl" + "z" or "Ctrl"+"c"(Depending on the machine that you are running) to end the program and go back to the terminal.


