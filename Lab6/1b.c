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
#include <linux/hrtimer.h>
#include <linux/ktime.h>

MODULE_LICENSE("GPL");

unsigned long timer_interval_ns = 200000;
static struct hrtimer hr_timer;
static int count = 0;
static struct task_struct *kthread1;

enum hrtimer_restart timer_callback(struct hrtimer *timer_for_restart){
	ktime_t currtime, interval;
	unsigned long overruns = 0, *ptr;

	currtime = ktime_get();
	interval = ktime_set(0, timer_interval_ns);
	overruns = hrtimer_forward(timer_for_restart, currtime, interval);

	//Access registers
	ptr = (unsigned long *)ioremap(0x3F20001C, 4096);

	if (count == 0){
		//Turn on the speaker
		*ptr = *ptr | 0x20;
		count++;
	} else {
		//Turn off the speaker
		*(ptr + 3) = *(ptr + 3) | 0x20;
		count = 0;
	}

	return HRTIMER_RESTART;
}



int kthread_fn(void *p){
	
	//Configure a timer
        ktime_t ktime = ktime_set(0, timer_interval_ns);
        hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
        hr_timer.function = &timer_callback;

	//Start the timer
	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);	

	while(1){
	
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

	kthread_stop(kthread1);
	hrtimer_cancel(&hr_timer);

}

module_init(thread_init);
module_exit(thread_cleanup);	
