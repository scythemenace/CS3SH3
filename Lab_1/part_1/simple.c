#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gcd.h>
#include <asm/param.h>

// Prototyping function
int simple_init(void);
void simple_exit(void);
/* This function is called when the module is loaded. */
int simple_init(void)
{
       printk(KERN_INFO "Loading Module\n");

	printk(KERN_INFO "HZ = %d\n",HZ);
	printk(KERN_INFO "jiffies = %lu\n",jiffies);

       return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
	printk(KERN_INFO "Removing Module\n");

        printk(KERN_INFO "%lu\n", gcd(3300,24));
}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

