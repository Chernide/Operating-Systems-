#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdio.h>

int main(){
	int ptr; 
	long int return_Val;
	return_Val = syscall(334, 100, -12, &ptr);
	if(return_Val == 0){
		printf("Success. Total is: %d\n", ptr);
	}
}
