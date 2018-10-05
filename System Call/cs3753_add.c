#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/uaccess.h>
asmlinkage long sys_cs3753_add(int fnum, int snum, int *ptr){
	int added;
	printk(KERN_ALERT"Num one: %d", fnum);
	printk(KERN_ALERT"Num two: %d", snum);  
	added = fnum + snum;
	put_user(added, ptr);
	printk(KERN_ALERT"Sum: %d\n", added);
	return 0;
}
