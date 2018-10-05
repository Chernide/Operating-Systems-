#include <linux/fs.h> 
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h> 
#include <linux/slab.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denver Chernin");


#define BUFF_SIZE 1024
char device_buffer[BUFF_SIZE];
int open_count = 0;
int close_count = 0;
void *memory_ptr;

//ptrFile = pointer to file
//char __user *buffer = user buffer for storing data
//size_t len = size of user buffer
//loff_t *pos_C = current_pos
int simple_open(struct inode *ptr_inode, struct file *ptrFile){
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	printk(KERN_ALERT "Device is now open\n");
	open_count++; 
	printk(KERN_ALERT "Device has been opened %d time(s)\n", open_count);
	return 0;
}

int simple_close(struct inode *ptr_inode, struct file *ptrFile){
		/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	printk(KERN_ALERT "Device is now closed\n");
	close_count++; 
	printk(KERN_ALERT "Device has been closed %d time(s)\n", close_count);
	return 0;
}


ssize_t simple_write(struct file *ptrFile, const char __user *buffer, size_t len, loff_t *pos_c){
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */
	int towrite; 
	int written; 
	int avail = BUFF_SIZE - *pos_c;
	if(avail > len){
		towrite = len; 
	} else {
		towrite = avail;
	}

	written = towrite - copy_from_user(device_buffer + *pos_c, buffer, towrite);

	if(written == 0){
		printk(KERN_ALERT "Out of space.\n");
	} else {
		*pos_c += written;
		printk(KERN_ALERT "Writing %d bytes\n", written);
	}
	return written;
}

ssize_t simple_read(struct file *ptrFile, char __user *buffer, size_t len, loff_t *pos_c){
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */
	int toRead = len; 
	int canRead = BUFF_SIZE - *pos_c;
	//int Read; 
	if(canRead == 0){
		printk(KERN_ALERT"You have reached eof\n");
		return toRead;
	}
	copy_to_user(buffer, device_buffer + *pos_c, len);
	printk(KERN_ALERT "Read %d bytes\n", len);

	*pos_c += len;
	return 0;
 }

loff_t simple_llseek (struct file *ptrFile, loff_t pos_c, int whence){
	/* Update open file position according to the values of offset and whence */
	loff_t n_pos = ptrFile->f_pos;
	switch(whence){
		case 0:
			n_pos = pos_c;
			break;
		case 1: 
			n_pos = ptrFile->f_pos + pos_c; 
			break;
		case 2: 
			n_pos = BUFF_SIZE - pos_c; 
			break;
	}
	if(n_pos > BUFF_SIZE) printk(KERN_ALERT"Position is greater than buffer size"); 
	if(n_pos < 0) printk(KERN_ALERT"Position is less than 0"); 
	ptrFile->f_pos = n_pos; 
	return n_pos;
}

struct file_operations simple_fops = {

	.owner   = THIS_MODULE,
	.open 	 = simple_open,
	.release = simple_close,
	.read	 = simple_read,
	.write	 = simple_write,
	.llseek  = simple_llseek,

};

static int device_init(void){
	printk(KERN_ALERT "In %s function\n", __FUNCTION__);
	int major_number = register_chrdev(412, "/dev/simple_character_device", &simple_fops);
	if(major_number <0){
		printk(KERN_ALERT "Failed to register");
	}
	memory_ptr = kmalloc(BUFF_SIZE, GFP_KERNEL);
	return 0;
}

static void device_exit(void){
	printk(KERN_ALERT "In %s function\n", __FUNCTION__);
	unregister_chrdev(412, "/dev/simple_character_device");
	kfree(memory_ptr);
}

module_init(device_init);
module_exit(device_exit);
