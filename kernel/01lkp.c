#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init lkp_init(void)
{
	printk("hello world\n");
	return 0;
}

static void __exit lkp_exit(void)
{
	printk("goodbye world\n");
}

module_init(lkp_init);
module_exit(lkp_exit);

MODULE_LICENSE("GPL");
