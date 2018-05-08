Kun Ye
V00871367
360 assignment2

*******************

"Airline Check-in System"
The check-in system includes 4 queues and 2 clerks. When a customer arrives, she enters a queue of the minimal
length. If there is a tie, she randomly selects one from the tie to enter. Once a customer enters a queue, she cannot
change or leave the queue. We assume that the length of the queue could be infinite and customers in a queue are
served FIFO. When a clerk is available, the clerk picks the customer from the queue of the longest length to serve.
If there is a tie, the clerk randomly selects a queue from the tie. When a clerk is available and there is no customer
in any queue, the clerk remains idle. We assume that the service time for a customer is known when the customer
enters the system.


*******************


In order to run my code there are two steps:
Step1: excute "make" in the terminal.  (if the code it's already been complied, just skip step1 and start from step 2 instead)
Step2: In order to excute,Type ./ACS <customer text file> in the terminal.


*******************
input restriction:
1. The first character specifies the unique ID of customers.
2. A colon(:) immediately follows the unique number of the customer.
3. Immediately following is an integer that indicates the arrival time of the customer.
4. A comma(,) immediately follows the previous number.
5. Immediately following is an integer that indicates the service time of the customer.
6. A newline (\n) ends a line.
you can follow the given input customers.txt

(if customers arrive time is too close or at the same time, the program may not able to be finished properly. This depends on your running environment, 
or you can set the arrive time of customers not that close to each other. Sometimes it may not able to be finished properly(depending on your running environment),
you can try to run the program again with the same input.)

note: you can use "Ctrl" + "z" or "Ctrl"+"c"(Depending on the machine that you are running) to end the program and go back to the terminal.

