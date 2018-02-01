//gpio -g mode 2 out  //Red LED
//gpio -g write 2 1   //Turn on Red LED
//gpio -g write 2 0   //Turn off Red LED

//gpio -g mode 16 in  //Push button one
//gpio -g read 16     //Outputs 0 when button is not pressed, 1 when pressed

//GPIO Function Select 0 address is 3F200000

#ifndef _KERNEL_
#define _KERNEL_
#endif

#ifndef MODULE
#define MODULE
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");

int init_module(void){
	unsigned long* ptr;

	//Open the memory location
	ptr = (unsigned long *)ioremap(0x3F200000, 4096);

	//Set all 4 LEDs to outputs
	*ptr = *ptr | 0x9240;

	//Turn all 4 LEDs on
	*(ptr + 7) = *(ptr + 7) | 0x3C;

	
	printk("MODULE INSTALLED\n");

	return 0;
}

void cleanup_module(void){
	unsigned long* ptr;

        //Open the memory location
        ptr = (unsigned long *)ioremap(0x3F200000, 4096);

	//Set all 4 LEDs to outputs
        *ptr = *ptr | 0x9240;

        //Turn all 4 LEDs off
        *(ptr + 10) = *(ptr + 10) | 0x3C;
	
	printk("MODULE REMOVED\n");
}
