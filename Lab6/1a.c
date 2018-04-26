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
#include <linux/gpio.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");

static struct task_struct *kthread1;

int kthread_fn(void *p){
	unsigned long *ptr;
		

	//Access correct memory location
	ptr = (unsigned long *)ioremap(0x3F20001C, 4096);

	while(1){
	
		//Turn the speaker on
		*ptr = *ptr | 0x20;

		//Wait
		udelay(200);

		//Turn off the speaker
		*(ptr + 3) = *(ptr + 3) | 0x20;

		//Wait again
		udelay(200);
	
		//Exit the kthread if it should
		if (kthread_should_stop()){
			do_exit(0);
		}
	



	}
}

int thread_init(void){
	char kthread_name[11] = "my_kthread";
	unsigned long *ptr;
		
	//Set the speaker as an output
	ptr = (unsigned long *)ioremap(0x3F200000, 4096);
        *ptr = *ptr | 0x40000;
	
	//Create a kthread
	kthread1 = kthread_create(kthread_fn, NULL, kthread_name);
	
	//Start the kthread
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
