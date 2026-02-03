/* simple_module.c - a simple template for a loadable kernel module in Linux,
   based on the hello world kernel module example on pages 338-339 of Robert
   Love's "Linux Kernel Development, Third Edition."
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/time64.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>

static ktime_t init_time;
static ktime_t exit_time;
/* init function - logs that initialization happened, returns success */
static int
simple_init(void)
{
    printk(KERN_ALERT "Module Initialized\n");
    init_time = ktime_get();
    return 0;
}

/* exit function - logs that the module is being removed */
static void
simple_exit(void)
{
    exit_time = ktime_get();
    struct timespec64 elapsed_time = ktime_to_timespec64(ktime_sub(exit_time, init_time));

    printk(KERN_ALERT "Time elapsed: %lld.%.9ld\n", elapsed_time.tv_sec, elapsed_time.tv_nsec);
    printk(KERN_ALERT "Module Unloaded\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Jonathan Rodriguez Gomez <j.rodriguezgomez@wustl.edu>");
MODULE_DESCRIPTION ("CSE 4202 Studio 6 module");
