#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#define USER_BUFFER_SIZE 1024

int main(){
	char user_buffer[USER_BUFFER_SIZE];
	int c = open("/dev/simple_character_device", O_RDWR);
	if(c < 0){
		printf("Failed to open Device\n");
	} else {
		printf("Device opened successfully\n");
	}
	char key = 0;
	int pos, whence, bytes = 0;
	printf("\nWelcome to the test application!\n");
	printf("This is to test my simple_character_device\n");
	while(1){
		printf("Options: \nPress r to read\nPress w to write\nPress s to seek\nPress e to exit\n \nEnter choice -> ");
		scanf("%c", &key);
		switch(key){
			case 'r':
				printf("Enter number of bytes you want to read: ");
				scanf("%d", &bytes);
				while(getchar() != '\n');
				char *read_buffer = malloc(bytes * sizeof(char));
				read(c, read_buffer, bytes);
				printf("Data read from the device: %s\n", read_buffer);
				free(read_buffer);
				//memset(user_buffer, 0, sizeof(user_buffer));
				break;
			case 'w':
				printf("Enter data you want to write to the device: ");
				scanf("%s", &user_buffer);
				while(getchar() != '\n');
				write(c, user_buffer, strlen(user_buffer));
				break;
			case 's':
				printf("Enter an offset value: ");
				scanf("%d", &pos);
				while(getchar() != '\n');
				printf("Enter a value for whence (third parameter): ");
				scanf("%d", &whence);
				while(getchar() != '\n');
				llseek(c, pos, whence);
				break;
			case 'e':
				printf("Exiting testing application\n");
				return 0;
			default:
				break;
		}
	}
	close(c);
}