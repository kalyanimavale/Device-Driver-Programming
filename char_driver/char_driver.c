#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>     //for file handling functins
#include<linux/uaccess.h> //copy_to_user(read()) , copy_from_user(write()) used because direct pointer access is not allowed in kernel

#define DEVICE_NAME "mychardev"   //device file name while registering the char driver

int major; //for major number which identify the driver code 
char kernel_buffer[1024]="Hello From Kernel!"; //allocate memory as in array form 

//OPEN
static int dev_open(struct inode *inode , struct file *file) //static retains value between each call don't want to change variable value again and again , accessible only within the file  
{
	printk(KERN_INFO "Device Opened\n");
	return 0;
}

//READ
static ssize_t dev_read(struct file *file, char __user *user_buffer, size_t len, 
		         loff_t  *offset)
{
	int bytes_to_copy=strlen(kernel_buffer);

	if(*offset>=bytes_to_copy)  //prevents infinite read loop
		return 0;
	
	if(copy_to_user(user_buffer,kernel_buffer,bytes_to_copy))  //kernel to user read() 
		return -EFAULT;

	*offset +=bytes_to_copy;

	printk(KERN_INFO "Data Read:%s\n",kernel_buffer);

	return bytes_to_copy;
}

//WRITE
static ssize_t dev_write(struct file *file, const char __user *user_buffer,
		         size_t len, loff_t *offset)
{

        if(copy_from_user(kernel_buffer,user_buffer,len)) //user to kernel write() system call
		return -EFAULT;

	kernel_buffer[len]='\0';

	printk(KERN_INFO "Data written:%s\n",kernel_buffer);

	return len;
}

//CLOSE 
static int dev_release(struct inode *inode ,struct file *file)
{
	printk(KERN_INFO "Device Closed\n");
	    return 0;
}

//File Operation Structure
struct file_operations fops={
	.open=dev_open,
	.read=dev_read,
	.write=dev_write,
	.release=dev_release,
};

//INIT
static int __init char_init(void)
{
	major=register_chrdev(0, DEVICE_NAME, &fops);

	if(major<0)
	{
		printk(KERN_ALERT "FAiled to register device\n");
		return major;
	}

	printk(KERN_INFO "Registered char device with major number %d\n", major);
	return 0;
}

//EXIT
static void __exit char_exit(void)
{
	unregister_chrdev(major,DEVICE_NAME);
	printk(KERN_INFO "Driver Removed\n");
}

module_init(char_init);
module_exit(char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kalyani Mavale");
MODULE_DESCRIPTION("Basic Character Driver");
















