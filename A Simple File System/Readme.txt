Kun Ye
V00871367
360 assignment3

*******************
"Sample File Systems"
This systems has four parts.
Part1
Displays basic information about the file system. Mainly rely
on the information from boot sector.

Part2
Display all the contents of root directory in the file system.

Part3
Copies a file from the file system (root directory) to the
current directory in Linux.(file name and extension are case senstive)

Part4
Copies a file from the current Linux directory into the root
directory of the file system.(file name and extension are case senstive)


*******************


In order to run my code there are two steps:
Step1: excute "make" in the terminal.  (if the code it's already been complied, just skip step1 and start from step 2 instead)
Step2: Run diskinfo ./diskinfo <file system image> in the terminal.
       Run disklist ./disklist <file system image> in the terminal.
       Run diskget ./diskget <file system image> <target file> in the terminal.(target file name and extension are case senstive)
       Run diskput ./diskget <file system image> <target file> in the terminal.(target file name and extension are case senstive)

       
*******************

Features:
diskinfo:
Displays information about the file system.

disklist:
Displays the contents of
the root directory in the file system.

diskget:
It copies a file from the file system to the current directory in Linux.(target file name and extension are case senstive)

diskput:
copies a file from the current Linux directory into the root directory of the file system.(target file name and extension are case senstive)


(The diskget and diskput are case senstive, if you get wrong output, check the file name and extension. When you use diskput puts a file into file system, 
and you use diskget to get ANS1.PDF from file system depends on your running environment, the ANS1.PDF may display uncomplete output.
In order to solve this problem, you can delete the current file system image, and copy a new file system image to the current directory.
Run diskget again, ANS1.PDF will display complete result.)

note: you can use "Ctrl" + "z" or "Ctrl"+"c"(Depending on the machine that you are running) to end the program and go back to the terminal.

