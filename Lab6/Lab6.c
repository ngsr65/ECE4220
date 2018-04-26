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
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

int mydev_id;
int sounddelay = 200;
unsigned long *ptr, data;

static irqreturn_t button_isr(int irq, void *dev_id){
	disable_irq_nosync(79);

	//Determine button pressed
	data = *(ptr + 16);

	switch(data){
		case 0x10000:
			sounddelay = 200;
		break;

		case 0x20000:
			sounddelay = 300;
		break;

		case 0x40000:
			sounddelay = 400;
		break;

		case 0x80000:
			sounddelay = 500;
		break;
	
		case 0x100000:
			sounddelay = 600;
		break;
	}

	//Clear Event detect status register
	*(ptr + 16) = *(ptr + 16) | 0x1F0000;

	enable_irq(79);

	return IRQ_HANDLED;
}

static struct task_struct *kthread1;



int kthread_fn(void *p){
	
	while(1){
	
		//Turn the speaker on
		*(ptr + 7) = *(ptr + 7) | 0x20;

		//Wait
		udelay(sounddelay);

		//Turn off the speaker
		*(ptr + 10) = *(ptr + 10) | 0x20;

		//Wait again
		udelay(sounddelay);
	
		//Exit the kthread if it should
		if (kthread_should_stop()){
			do_exit(0);
		}
	



	}
}

int thread_init(void){
	char kthread_name[11] = "my_kthread";
	int dummy = 0;		

	//Set the speaker as an output
	ptr = (unsigned long *)ioremap(0x3F200000, 4096);
        *ptr = *ptr | 0x40000;

	//Set buttons as inputs
	*(ptr + 1) = *(ptr + 1) | 0x00000000;
	*(ptr + 2) = *(ptr + 2) | 0x0;

	//Enable pull down resistors
	*(ptr + 37) = *(ptr + 37) | 0x1;
	udelay(100);
	*(ptr + 38) = *(ptr + 38) | 0x1F0000;
	udelay(100);
	*(ptr + 37) = *(ptr + 37) | 0x0;
	*(ptr + 38) = *(ptr + 38) | 0x0;

	//Enable rising edge detect 
	*(ptr + 31) = *(ptr + 31) | 0x1F0000;
	
	//Create a kthread
	kthread1 = kthread_create(kthread_fn, NULL, kthread_name);
	
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);

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
	
	free_irq(79, &mydev_id);
}

module_init(thread_init);
module_exit(thread_cleanup);	
