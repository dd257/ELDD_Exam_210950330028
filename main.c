#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/uaccess.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/types.h>
#include<linux/cdev.h>
#include<linux/ioctl.h>
#include"ioctl.h"

int device_open(struct inode *,struct file *);
int device_release(struct inode *,struct file *);
ssize_t device_read(struct file *,char __user *,size_t,loff_t *);
ssize_t device_write(struct file *,const char __user *,size_t,loff_t *);
long device_ioctl(struct file *,unsigned int,unsigned long);
dev_t mydev;

struct file_operations fops=
{
    .owner=THIS_MODULE,
    .read=device_read,
    .write=device_write,    
    .unlocked_ioctl=device_ioctl,
    .open=device_open,
    .release=device_release,
};

struct stats
{
    int size;
    char buff[100];
    int r_w;
};

struct stats *tosend;

struct cdev *my_cdev;

int device_open(struct inode *inode,struct file *filp)
{
    printk("Open call");
    return 0;
}

int device_release(struct inode *inode,struct file *filp)
{
    printk("Release call");
    return 0;
}

ssize_t device_read(struct file *filp,char __user *Ubuff,size_t count,loff_t *offp)
{
    char Kbuff[100];

    sprintf(Kbuff,"%s",tosend->buff);

    int result;

    result=copy_to_user(Ubuff,Kbuff,sizeof(Kbuff));

    if (result<0)
    {
        printk("Copy to User failed\n");
        return -1;
    }

    printk("Data read by User %s: %s\n",(result>0)?("Incomplete"):("Complete"),Ubuff);

    return count-result;
}

ssize_t device_write(struct file *filp,const char __user *Ubuff,size_t count,loff_t *offp)
{
    char Kbuff[100];
    int result;

    result=copy_from_user(Kbuff,Ubuff,sizeof(Ubuff));

    if (result<0)
    {
        printk("Copy from User failed\n");
        tosend->r_w=0;
        return -1;
    }

    printk("Data read from User %s: %s\n",(result>0)?("Incomplete"):("Complete"),Ubuff);
    
    sprintf(tosend->buff,"%s",Ubuff);
    tosend->r_w=1;
    tosend->size=sizeof(Ubuff);

    return count-result;
}

long device_ioctl(struct file *filp,unsigned int cmd, unsigned long arg)
{
    char Ubuff[100];
    int result;
   
    switch(cmd)
    {
       case GETSTATS:
            copy_to_user(Ubuff,tosend,sizeof(tosend));
            break;

        case DEFAULT:
            copy_from_user(Ubuff,(char __user*)arg,sizeof(arg));
            printk("Default message from User: %s",Ubuff);
            break;
        
        default:
            printk(KERN_ALERT "Command not found\n");

    }

    return 0;
}

static int __init insert(void)
{
    int result,major,minor;
    
    printk(KERN_ALERT "\nInserting Module\n");
    result=alloc_chrdev_region(&mydev,0,1,"/dev/lddexam");

    if(result<0)
    {
        printk("Error registering device\n");
        return result;
    }
    
    major=MAJOR(mydev);
    minor=MINOR(mydev);

    printk("Major=%d\nMinor=%d\n",major,minor);

    my_cdev=cdev_alloc();
    my_cdev->ops=&fops;

    result=cdev_add(my_cdev,(int)mydev,1);
    
    if(result<0)
    {
        printk("Error adding device");
        unregister_chrdev_region(mydev,1);
        printk("Device unregistered");
        return -1;
    }
    return 0;
}

static void __exit remove(void)
{
    unregister_chrdev_region(mydev,1);

    cdev_del(my_cdev);

    printk(KERN_ALERT "\nModule removed");    
}


module_init(insert);
module_exit(remove);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RAHUL");
MODULE_DESCRIPTION("ELDD exam");