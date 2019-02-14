#include <linux/init.h>  /*most of the kernel utilities are in init.h*/
#include <linux/module.h> /*utilities related to loadable kernel modules are in module.h*/

#include <linux/fs.h> /*linux filesystem utilities to be used in this device driver*/

MODULE_LICENSE("GPL"); //to make sure kernel does not crack (taint) 

int count = 1;
module_param(count, int, 0644); //To check the working of paramter being 'count' passed at run-time.


/*************** Files operations performed on this char device driver.**************/

/**
	sim_ch_dd_open() function is accessed when a read/write is performed using 
	this character device.  
*/
int sim_ch_dd_open (struct inode *pinode, struct file *pfile)
{
 printk(KERN_ALERT "Inside simple_charac_dd OPEN function\n");
 return 0;
}


/**
	sim_ch_dd_read() is accessed when a read is performed using this character device. 
*/
ssize_t sim_ch_dd_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)	
{
 printk(KERN_ALERT "Inside simple_charac_dd READ function\n");
 return 0;
}


/**
	sim_ch_dd_write() is accessed when a write is performed using this character device. 
*/
ssize_t sim_ch_dd_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
 printk(KERN_ALERT "Inside simple_charac_dd WRITE function\n");
 return length; //tell the user that we have written the data in buffer of size length
}


/**
	sim_ch_dd_release() is accessed when a read/write is finished using this character device. 
*/
int sim_ch_dd_release (struct inode *pinode, struct file *pfile)
{
 printk(KERN_ALERT "Inside simple_charac_dd RELEASE function\n");
 return 0;
}

/*****************************************************************************************/


/**
	sim_ch_dd_flag() is called when paramter is passed to this device during run-time.
*/

int sim_ch_dd_flag (int scdd_flag)
{
 printk(KERN_ALERT "Inside simple_charac_dd FLAG function, flag = %d\n",scdd_flag);
 return 0;
}


/** struct file_operations contains all the operations that can be performed on the 
		character device.  
*/
struct file_operations sim_ch_dd = {
	.owner = THIS_MODULE,
	.open = sim_ch_dd_open,
	.read = sim_ch_dd_read,
	.write = sim_ch_dd_write,
	.release = sim_ch_dd_release,
	.check_flags = sim_ch_dd_flag,
};


/** init func to initiate the module to be loaded. Iti called when a USB device 
		is plugged into the system. The double underscore (__) before init implies 
		that the code for init function will be removed from RAM after its call, because 
		it will never be used again. 
*/
__init int simple_charac_dd_init(void)
{
 printk(KERN_ALERT "Inside simple_charac_dd init function\n");
 /*Register the device using following function from -- register_chrdev("Device number", Device name", "file_operations_variable")*/
 /*file_operations_variable is of type struct file_operations in fs.h file. */
 /*it has all the file operations performed on this device*/ 
 register_chrdev(240, "simple_charac_dd", &sim_ch_dd); //now all the services to the corr device will be served by this registered driver
 sim_ch_dd_flag(count);
 return 0;
}


/** exit func to cleanup after the module is unloaded. It is called after the USB 
		device is unplugged from the system. 
*/
void simple_charac_dd_exit(void)
{
  printk(KERN_ALERT "Inside simple_charac_dd exit function\n");
  sim_ch_dd_flag(count);
  unregister_chrdev(240, "simple_charac_dd"); 
}


/** Two macros from init utility to tell the kernel which of the above functions. 
*/
module_init(simple_charac_dd_init);
module_exit(simple_charac_dd_exit);



/*NOTE:- After inserting module for dd, we have to create a node for it in /dev/.. with the name of this module. Linux treats everything
as a file. To perform functions (I/O operations) of this module after it is loaded, linux will use this node (file).
So, the cmd-> echo "test" > /dev/simple_charac_dd   is a system call for writing "test" into the device  
    the cmd-> cat /dev/simple_charac_dd   is a system call for reading from the device  which are executed only upon creation of this file.
Unloading the module does not delete this file. Reloading the module does necessarily mean creation of this file again, unless it was removed. */







 	
