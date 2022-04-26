#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/atomic.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/device.h>
#include <linux/kfifo.h>
#include <linux/miscdevice.h>

#include <linux/slab.h>
#include <linux/gfp.h>
#include <linux/poll.h>

#define DEV_NAME "mydevice"
#define BUFFER_SIZE 32
DEFINE_KFIFO(FIFOBuffer, char, BUFFER_SIZE);
struct _BlockDevice{
	const char *name;
	struct device *device;
	struct miscdevice *miscdev;
	wait_queue_head_t read_queue;
	wait_queue_head_t write_queue;
};

struct _BlockDevice * BlockDevice;
int ret, actual_readed, actul_write;
struct _BlockDevice temp;
// open device
static int mydevice_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "open mydevice\n");
	return 0;
}

// release device
static int mydevice_release(struct inode * inode, struct file * filp){
	printk(KERN_INFO "release mydevice\n");
	return 0;
}

// read device
static ssize_t mydevice_read(struct file *file, char __user *buf, size_t count, loff_t *offset){
	// don't have data to read
	if(kfifo_is_empty(&FIFOBuffer)){	
		if(file->f_flags & O_NONBLOCK)
			return -EAGAIN;
		// wait until condition is met
		ret = wait_event_interruptible(BlockDevice->read_queue, !kfifo_is_empty(&FIFOBuffer));
	}
	
	// copy read data to user's buffer
	ret = kfifo_to_user(&FIFOBuffer, buf, count, &actual_readed);

	// when queue is not full, call write process to write data
	if(!kfifo_is_full(&FIFOBuffer)){
		wake_up_interruptible(&BlockDevice->write_queue);
	}
	return actual_readed;
}

// write device
static ssize_t mydevice_write(struct file * file, const char __user *buf, size_t count, loff_t *offset){
	if(kfifo_is_full(&FIFOBuffer)){
		if(file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		ret = wait_event_interruptible(BlockDevice->write_queue, !kfifo_is_full(&FIFOBuffer));
	}
	ret = kfifo_from_user(&FIFOBuffer, buf, count, &actul_write);

	if(!kfifo_is_empty(&FIFOBuffer))
		wake_up_interruptible(&BlockDevice->read_queue);
	return actul_write;
}

// file struct
static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = mydevice_open,
	.release = mydevice_release,
	.read = mydevice_read,
	.write = mydevice_write,
};

static struct miscdevice miscDeviceFIFOBlock = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEV_NAME,
	.fops = &fops,
};

// init
static int __init mydevice_init(void){
	ret = misc_register(&miscDeviceFIFOBlock);
	BlockDevice = &temp;
	BlockDevice->miscdev = &miscDeviceFIFOBlock;
	init_waitqueue_head(&BlockDevice->read_queue);
	init_waitqueue_head(&BlockDevice->write_queue);
	return 0;
}

// exit
static void  __exit mydevice_exit(void){
	misc_deregister(&miscDeviceFIFOBlock);
}

module_init(mydevice_init);
module_exit(mydevice_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MRX");
MODULE_DESCRIPTION("character device");
