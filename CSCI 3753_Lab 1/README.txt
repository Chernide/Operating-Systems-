Name: Denver Chernin
Email: dech1905@colorado.edu


A) /home/kernel/linux-hwe-4.15.0/arch/x86/kernel/cs3752_add.c

	- This files contains the source code for the system call that adds two numbers and returns the value back into user-space using a pointer. 
	It also provides printks so that in the "syslog" or "dmesg" it is clear what is happening in kernel space. 

B) /home/kernel/linux-hwe-4.15.0/arch/x86/kernel/Makefile

	- This files contains "obj-y +=cs3753_add.o" -> the -y tells kbuild to compile a system call that has the same name as the .o file but is a .c file. 

C) /home/kernel/linus-hwe-4.15.0/x86/entry/syscalls/syscall_64.tbl

	- This file maps each system call to a unique number identification which allows the user to know what number to 
	pass syscall() in order to call an individual system call. The first column is that unique ID, the second columnn is for what system 
	call is compatible with (64 or 32-bit), the third column is a name for the system call, and the final column is the name of our function. 

D) /home/kernel/linus-hwe-4.15.0/include/linux/syscalls.h

	- This file is a header file with a prototype of our system call included in order to have access to other files from our system call. 

E) /var/log/syslog 

	- This file is the same as 'dmesg' except within a file. This file has a record of many recent system calls (including printk and other kernel space outputs).

F) /Desktop/CSCI3753/PA1/cs3753_add_test.c

	- The file tests the system call that resides within my kernel. It includes the syscall.h header file so that we can access our system call. 
	It calls the system call using the number assigned in the syscall_64.tbl. It provides the three parameters needed, two numbers to be added 
	together, and an address in user space that the answer will be put into. The system call returns zero if there is no error so if the 
	return_Val is zero it will print "success" and the sum of the two numbers. 


Standard build tree:

1) System call source code

/home/kernel/linux-hwe-4.15.0/arch/x86/kernel/cs3752_add.c

2) Files that the system call depends on to compile and run

/home/kernel/linux-hwe-4.15.0/arch/x86/kernel/Makefile
/home/kernel/linus-hwe-4.15.0/x86/entry/syscalls/syscall_64.tbl
/home/kernel/linus-hwe-4.15.0/include/linux/syscalls.h

3) Testing

/Desktop/CSCI3753/PA1/cs3753_add_test.c
/var/log/syslog OR demsg


To test system calls:

1) Make sure your kernel is compiled (-j4 because I have given more cores to my VM)

sudo make -j4 CC="ccache gcc"
sudo make -j4 modules_install
sudo make -j4 install
sudo reboot

2) Compile testing code

gcc -o add c3753_add_testing.c

3) Run testing code (and make sure output is correct)

./add

4) Check dmesg or /var/log/syslog to see kernel space outputs since our system call should have printk to print to these logs. 
Make sure your system call source code prints with "KERN_ALERT" which highlights the prints from your system call in red to \
make them easier to find in the logs.