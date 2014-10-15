#include <asm/io.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#include "utils.h"

#define GPIO_USRLED             199
#define GPIO_USRBTN1            46
#define GPIO_USRBTN2            47

#define MODULE_NAME             "zda_n_zpv_module"

MODULE_LICENSE ("GPL");
MODULE_DESCRIPTION ("PLS NO COPYPASTERINO");
MODULE_AUTHOR ("ZDA&ZPV");

static int __init init_routine(void);
static void __exit exit_routine(void);

module_init(init_routine);
module_exit(exit_routine);

int usrbtn1_irq_num = 0;
int usrbtn2_irq_num = 0;
volatile int usrled_val = 0;

struct interrupt_context
{
    spinlock_t lock;
};

struct interrupt_context *int_context = NULL;

static irqreturn_t usrbtn1_irq_handler(int irq, void *dev_id)
{
    struct interrupt_context *context = (struct interrupt_context*)dev_id;	
    spin_lock_irq(&context->lock);
    spin_unlock_irq(&context->lock);
    return( IRQ_HANDLED );
}

static int __init init_routine(void)
{
    int result;
    
    TRACE_N("Init routine started");
    // Init context
    int_context = kmalloc(sizeof(struct interrupt_context), GFP_KERNEL);
    if(int_context == NULL)
    {
        TRACE_F("failed to allocate %d bytes", sizeof(struct interrupt_context));
        return -1;
    }
    spin_lock_init(&int_context->lock);
    
    // Setup GPIO_USRBTN1
    if(FALSE == gpio_is_valid(GPIO_USRBTN1)
    {
        TRACE_F("gpio GPIO_USRBTN1 is not valid");
        goto init_failed;
    }
    
    if(0 != (result = gpio_request(GPIO_USRBTN1, MODULE_NAME)))
    {
        TRACE_F("gpio_request for GPIO_USRBTN1 failed on error %d", result);
        goto init_failed;
    }
    
    if(0 != (result = gpio_direction_input(GPIO_USRBTN1)))
    {
        TRACE_F("gpio_direction_input for GPIO_USRBTN1 failed on error %d", result);
        goto init_failed_btn1;
    }
    
    if(INVALID_IRQ == (usrbtn1_irq_num = gpio_to_irq(GPIO_USRBTN1)))
    {
        TRACE_F("failed to get irq from GPIO_USRBTN1");
        goto init_failed_btn1;
    }
    
    if(0 != (result = request_irq(usrbtn1_irq_num, usrbtn1_irq_handler, IRQF_DISABLED | IRQF_TRIGGER_FALLING, MODULE_NAME, int_context)))
    {
        TRACE_F("request_irq for GPIO_USRBTN1 failed on error %d", result);
        goto init_failed_btn1;
    }
    
    TRACE_S("load succeed");
    return 0;

init_failed_btn1:
    if(usrbtn1_irq_num != 0)
    {
        free_irq(usrbtn1_irq_num, int_context);
    }
    gpio_free(GPIO_USRBTN1);
init_failed:
    free(int_context);
    int_context = NULL;
    return -1;
}
static void __exit exit_routine(void)
{
    // free IRQs
    free_irq(usrbtn1_irq_num, int_context);
    
    // release GPIO control
    gpio_free(GPIO_USRBTN1);
    
    // free context
    kfree(int_context);
    int_context = NULL;
    TRACE_S("unload succeed");
}
