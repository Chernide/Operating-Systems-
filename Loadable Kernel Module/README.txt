Denver Chernin - dech1905@colorado.edu
CSCI 3753 
Programming Assignment 2 - README

Device_Driver.c (device driver module source code)
	- File contains the source code that is compiled to the .ko module
	- Contains file operation struct that links open,write,read,etc to the functions I declared. (Total of 5 operations allowed)
	- Contains 7 functions
		- Open
			- Prints number of times the file has been opened using global counting variable 
		- Close
			- Prints number of times the file has been closed using global counting variable
		- Write
			- Takes in 4 parameters: file, user buffer, length, position pointer
			- Calculates the amount of available bytes using the buffer size and position pointer and checks that number against the length of what the use wants to write. 
			- Copies from user to the file
				- Copy_from_user returns number of bytes that failed to write
			- It then checks if copy_from_user returns the same number as len/available and if it does you are out of space
			- If not it incremements the position the number of bytes sucessfully written and prints to the kernel
		- Read
			- Takes in 4 parameters: file, user buffer, length, position pointer
			- Finds out how many bytes can be read using buffer size and the current position pointer
				- If 0 then prints to the kernel that you are at the EOF
			- Otherwise, uses copy_to_user to copy to the user buffer the number of bytes that user wants to read
			- Prints to the kernel the number of bytes read
			- Increments position by number of bytes read
		- Seek
			- Takes in 3 parameters: file, position pointer, whence
			- Finds the current position within file
			- Switch statement based on whence
				- 0 = set position to given position pointer value
				- 1 = set posotion to the current position + given position pointer value
				- 2 = set position to the buffer size - given position pointer value
			- Checks if you are going out of bounds
			- Sets position within file to new value
		- Init
			- Registers the character driver
				- Making sure that it registers correctly by returning a major number greater than 0 
			- Allocates the memory necessary for the file
		- Exit
			- Unregisters the character driver
			- Frees memory that was allocated within the Init function

Device_Driver_Test.c (Testing character device modules)
	- Creates a user space buffer
	- Opens the device and checks if it opens sucessfully
	- Starts loop (which ends when user enters 'e')
		- Case 'read'
			- Gets user input on number of bytes wanted to be read
			- Will create a buffer and allocate the memory for that size
			- Calls the read function
			- Then prints out read information
			- Frees the allocated memory so that the next time through it is a clean read
		- Case 'write'
			- Gets user input on text to write to file
			- Calls write function
		- Case 'seek'
			- Gets user input on position and whence
			- Calls seek
		- Case 'exit'
			- Test file quits

Makefile
	- This files contains "obj-m :=Device_Driver.o" -> the -m tells kbuild to compile a loadable kernel module that has the same name as the .o file but is a .c file. 


Testing and Running:
	1) Find major number that doesn't exist already by looking through kernel documentation
	2) Make driver file
		- 'sudo mknod -m 777 /dev/simple_character_drive c 412 0"'
			- '-m' -> module
			- 777 -> read,write,execute by all owner,group, other
			- 'c' -> defines this as a character device
			- 412 -> choosen major number
			- 0 -> choosen minor number
	3) Compile module
		- 'sudo make -C /lib/modules/$(uname -r)/build M=$PWD modules'
	4) Install LKM
		- 'sudo insmod Device_Driver.ko'
	5) Make sure it is running
		- 'cat /proc/devices'
	6) Compile and run testing program
		- 'gcc -o Driver Device_Driver_Test.c'
		- './Driver'
	7) Check kernel output
		- 'dmesg'
		- '/var/log/syslog'
	8) Uninstall LKM
		- 'sudo rmmod Device_Driver'
