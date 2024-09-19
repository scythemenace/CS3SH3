/*
 * lsb_release -a:-
 * seconds.c 
 * Distributor ID: Ubuntu
 * Description:    Ubuntu 24.04.1 LTS 
 * Release:        24.04 
 * Codename:       noble
 * 
 * uname -r: 6.8.0-41-generic
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

/**
 * Function prototypes
 */
ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);
int proc_init(void);
void proc_exit(void);

// Global variables for showcasing start_time and end_time, by time we mean the jiffies which will give us the time when divided
// by the HZ rate.
unsigned long start_jiffies;
unsigned long later_jiffies;

static const struct proc_ops my_proc_ops = {
        .proc_read = proc_read,
};

/* This function is called when the module is loaded. */
int proc_init(void)
{

        // creates the /proc/hello entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &my_proc_ops);
        
        /*Since proc_init is called when the module first loads, we are initializing the value for our start_jiffies here*/
        start_jiffies = jiffies;

        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {

        // removes the /proc/hello entry
        remove_proc_entry(PROC_NAME, NULL);

        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/hello is read.
 * 
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 *
 * params:
 *
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
        int rv = 0;
        static int completed = 0;

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;
        
        /* Basically whenever someone will write cat /proc/seconds, a new value will be assigned
          to the later_jiffies variables so that we can have the current jiffies value since the kernel
          was booted
         */
        later_jiffies = jiffies;
        unsigned long difference_in_jiffies = later_jiffies - start_jiffies; // Denotes the difference in jiffies from current to the value assigned when kernel was booted
        unsigned long elapsed_time = difference_in_jiffies/HZ;
        /*The line below declares a character array (elapsed_time_in_char) of size buffer size. This array will be used to store
         the formatted string representation of the elapsed time (in seconds) that has passed since the kernel module was loaded.*/
        char elapsed_time_in_char[BUFFER_SIZE];
        /*The (snprintf) function writes the value of elapsed_time into the elapsed_time_in_char array, formatted as a string
         using the "%lu" format specifier (used for types unsigned long).*/
        rv = snprintf(elapsed_time_in_char, sizeof(elapsed_time_in_char), "Seconds elapsed: %lu\n", elapsed_time); 
 
        // copies the contents of buffer to userspace usr_buf
        // This shows a warning as copy_to_user returns 0 for success and we aren't allocating it so it can be ignored
        copy_to_user(usr_buf, elapsed_time_in_char, rv); 

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello Module");
MODULE_AUTHOR("SGG");

