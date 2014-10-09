#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include "utils.h"
#include <linux/delay.h>
#include <linux/slab.h>

#define CLOCK_RESOLUTION	100

#define GPIO_USRLED  199
#define GPIO_USRBTN1 46
#define GPIO_USRBTN2 47

#define TZIC_BASE 0xFFFC000 
#define TZIC_WAKEUP0 (TZIC_BASE + 0x0E00)
#define TZIC_WAKEUP1 (TZIC_BASE + 0x0E04)
#define TZIC_WAKEUP2 (TZIC_BASE + 0x0E08)
#define TZIC_WAKEUP3 (TZIC_BASE + 0x0E0C)

#define CCM_BASE 0x53FD4000
#define CCM_CCGR2 CCM_BASE + 0x70
#define CCM_CLPCR (CCM_BASE + 0x4C)

#define EPIT1_BASE 0x53FAC000 
#define EPIT1_EPITCR	(EPIT_BASE + 0x0)
#define EPIT1_EPITSR	(EPIT_BASE + 0x4)
#define EPIT1_EPITLR 	(EPIT_BASE + 0x8)
#define EPIT1_EPITCMRP	(EPIT_BASE + 0xC)

#define EPIT1_EPITCR_32KHZ		(3 << 24)
#define EPIT1_EPITCR_STOPEN		(1 << 21)
#define EPIT1_EPITCR_WAITEN		(1 << 19)
#define EPIT1_EPITCR_DBGEN		(1 << 18)
#define EPIT1_EPITCR_IOVW		(1 << 17)
#define EPIT1_EPITCR_SWR		(1 << 16)
#define EPIT1_EPITCR_PRESCALER	(0 << 4)
#define EPIT1_EPITCR_RLD		(1 << 3)
#define EPIT1_EPITCR_OCIEN		(1 << 2)
#define EPIT1_EPITCR_ENMOD		(1 << 1)
#define EPIT1_EPITCR_EN			(1 << 0)

#define EPIT1_EPITSR_OCIF		(1 << 0)


#define MODULE_NAME "example_module"

MODULE_LICENSE ("GPL");
MODULE_DESCRIPTION ("Simple linux kernel module example.");
MODULE_AUTHOR ("Student");

static int __init init_routine(void);
static void __exit exit_routine(void);

module_init(init_routine);
module_exit(exit_routine);

int usrbtn1_irq_num;
volatile int usrled_val = 0;
struct interrupt_context *int_context = NULL;

struct interrupt_context
{
	int counter;
	int Q;
};


struct epit_reg
{
	int cr;
	int sr;
	int lr;
	int cmpr;
	int cntr;
};

static struct epit_reg *epit; 

static irqreturn_t usrbtn1_irq_handler( int irq, void *dev_id )
{
    //usrled_val = 1 & ~usrled_val;
    //gpio_set_value(GPIO_USRLED, usrled_val);
    return( IRQ_HANDLED );
}

static irqreturn_t epit1_irq_handler( int irq, void *dev_id)
{
	struct interrupt_context *context = (struct interrupt_context*)dev_id;
	if(context->counter == 0)
	{
		usrled_val = 1 & ~usrled_val;
	    gpio_set_value(GPIO_USRLED, usrled_val);
	}
	else if(context->counter == context->Q)
	{
		usrled_val = 1 & ~usrled_val;
	    gpio_set_value(GPIO_USRLED, usrled_val);
	}

	if(++(context->counter) >= CLOCK_RESOLUTION)	context->counter = 0;

	iowrite32(ioread32(&epit->sr) | EPIT1_EPITSR_OCIF, &epit->sr);
	return (IRQ_HANDLED);
}

static int __init init_routine(void)
{
    int result = 0, i;
	void *ccm = NULL;
	void *tzic = NULL;
	
	int_context = kmalloc(sizeof(struct interrupt_context),GFP_KERNEL);
	if(int_context == NULL)
	{
		printk(KERN_INFO "fuck\n");
		return -1;
	}
	int_context->counter = 0;
	int_context->Q = 30;

    printk( KERN_INFO "%s: initialization.\n", MODULE_NAME);

    result = -1 + gpio_is_valid( GPIO_USRLED );
    __ERROR_RETURN(result, "USRLED gpio_is_valid");
    gpio_free( GPIO_USRLED );
    result = gpio_request( GPIO_USRLED, MODULE_NAME );
    __ERROR_RETURN(result, "USRLED gpio_request");
    result = gpio_direction_output( GPIO_USRLED, usrled_val);
    __ERROR_RETURN(result, "USRLED gpio_direction_output");
    
    result = -1 + gpio_is_valid( GPIO_USRBTN1 );
    __ERROR_RETURN(result, "GPIO_USRBTN1 gpio_is_valid");
    gpio_free( GPIO_USRBTN1 );
    result = gpio_request( GPIO_USRBTN1, MODULE_NAME );
    __ERROR_RETURN(result, "GPIO_USRBTN1 gpio_request" );
    result = gpio_direction_input( GPIO_USRBTN1 );
    __ERROR_RETURN(result, "GPIO_USRBTN1 gpio_direction_input");
    usrbtn1_irq_num = gpio_to_irq( GPIO_USRBTN1 );
    result = request_irq( usrbtn1_irq_num, usrbtn1_irq_handler, IRQF_TRIGGER_FALLING, MODULE_NAME, NULL );
    __ERROR_RETURN(result, "GPIO_USRBTN1 request_irq");
    gpio_free( GPIO_USRBTN1 );
// Geather info

	tzic = ioremap(TZIC_WAKEUP0, 4 * sizeof(unsigned int));
	if(tzic == NULL)
	{
		printk(KERN_INFO "ioremap tzic failed");
		return -1;
	}

	for(i = 0; i < 16; i+=4)
	{
		printk(KERN_INFO "TZIC_WAKEUP%d = 0x%x\n", i/4, ioread32(tzic + i));
	}

	iounmap(tzic);

// CLOCK TO TIMER
	ccm = ioremap(CCM_CLPCR, sizeof(unsigned int));

	if(ccm == NULL)
	{
		printk(KERN_INFO "ioremap ccm failed");
		return -1;
	}

	iowrite32((~0x3) & ioread32(ccm), ccm);
	
	printk(KERN_INFO "CCM_CLPCR = 0x%x\n", ioread32(ccm));
	iounmap(ccm);

	ccm = ioremap(CCM_CCGR2, sizeof(unsigned int));

	if(ccm == NULL)
	{
		printk(KERN_INFO "ioremap ccm failed\n");
		return -1;
	}
	
	iowrite32(0x0C | ioread32(ccm), ccm);
	iounmap(ccm);

// WRITING TO THE TIMER REGISTERS
	request_mem_region(EPIT1_BASE, sizeof(struct epit_reg), MODULE_NAME);

	epit = ioremap(EPIT1_BASE, sizeof(struct epit_reg));
	if(epit == NULL)
	{
		printk(KERN_INFO "ioremap epit failed\n");
		return -1;
	}
	iowrite32(ioread32(&epit->cr) & 0xFC100000, &epit->cr);
	iowrite32(0x38E / CLOCK_RESOLUTION, &epit->lr);
	iowrite32(0x0, &epit->cmpr);
	iowrite32(EPIT1_EPITCR_32KHZ | EPIT1_EPITCR_IOVW | EPIT1_EPITCR_PRESCALER | EPIT1_EPITCR_OCIEN | EPIT1_EPITCR_RLD | EPIT1_EPITCR_ENMOD | EPIT1_EPITCR_WAITEN, &epit->cr);
	printk( KERN_INFO "epit_cr = %x\n", ioread32(&epit->cr));
	
// IRQ
	if( 0 != request_irq(40, epit1_irq_handler, 0, MODULE_NAME, int_context))
	{
		printk(KERN_INFO "request_irq failed\n");
		return -EBUSY;	
	}

	enable_irq(40);
//Enable epid	
	iowrite32(ioread32(&epit->cr) | EPIT1_EPITCR_EN, &epit->cr);
    printk( KERN_INFO "%s: started.\n", MODULE_NAME);

/*	void *asd;
	request_mem_region(0x53FD4054, 4, MODULE_NAME);

	asd = ioremap(0X53FD4054, 4);
	if(asd == NULL)
	{
		printk(KERN_INFO "ioremap epit failed\n");
		return -1;
	}
	printk(KERN_INFO "mode is 0x%x\n", ioread32(asd));
	*/
    return 0;
}
static void __exit exit_routine(void)
{
	printk(KERN_INFO "cnt = 0x%x\n", ioread32(&epit->cntr));
    free_irq(usrbtn1_irq_num, NULL);
	free_irq(40, int_context);
	iowrite32(ioread32(&epit->cr) & 0xFC100000, &epit->cr);
	iounmap(epit);
	release_mem_region(EPIT1_BASE, sizeof(struct epit_reg));
    gpio_free( GPIO_USRLED );
    gpio_set_value(GPIO_USRLED, 1);
	kfree(int_context);
	int_context = NULL;
    printk( KERN_INFO "%s: stopped.\n", MODULE_NAME);
}

// PROC FILE
//int example_read_procmem(char *buf, char **start, )


