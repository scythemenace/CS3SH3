#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gcd.h>
#include <linux/sched.h>

// Prototyping functions
int simple_init(void);
void simple_exit(void);
void print_init_PCB(void);

void print_init_PCB(void) {
    printk(KERN_INFO "Loading Module\n");
    struct task_struct* task_start = &init_task;
    printk(KERN_INFO "init_task pid: %d\n", task_start->pid);
    printk(KERN_INFO "init_task state: %u\n", task_start->__state);
    printk(KERN_INFO "init_task flags: %x\n", task_start->flags);
    printk(KERN_INFO "init_task runtime priority: %d\n", task_start->rt_priority);
    printk(KERN_INFO "init_task process policy: %d\n", task_start->policy);
    printk(KERN_INFO "init_task tgid: %d\n", task_start->tgid);
}

int simple_init(void) {
    print_init_PCB();
    return 0;
}

void simple_exit(void) {
    printk(KERN_INFO "Removing module\n");
    printk(KERN_INFO "GCD of 3300 and 24 is: %lu\n", gcd(3300, 24));
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple module");
MODULE_AUTHOR("sgg");
