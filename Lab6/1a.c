#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

static struct task_struct *kthread1;

int kthread_fn(void *ptr){
	unsigned long j0, j1, *ptr;
	int count = 0;
	
	j0 = jiffies;
	j1 = j0 + 10*HZ;

	ptr = (unsigned long *)ioremap(0x3F200004, 4096);
	*ptr = *ptr | 0x40000;
	

	while(1){
		
		if (kthread_should_stop()){
			do_exit(0);
		}
	



	}
}

int thread_init(void){
	char kthread_name[11] = "my_kthread";
	
	kthread1 = kthread_create(kthread_fn, NULL, kthread_name);
	
	if (kthread1){
		wake_up_process(kthread1);
	}

	return 0;
}

void thread_cleanup(void){
	int ret;
	
	ret = kthread_stop(kthread1);

	if (!ret){
		printk("Kthread stopped");
	}
}

module_init(thread_init);
module_exit(thread_cleanup);	
