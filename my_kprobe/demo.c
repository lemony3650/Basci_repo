#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobe.h>

static struct kprobe kp = {
    .symbol_name = "blkdev_ioctl",
};

static int handler_pre(struct kprobe *p,struct pt_regs *regs)
{
    dump_stack();
    return 0;
}

static void handler_post(struct kprobe *p,struct pt_regs *regs,
                        unsigned long flags)
{

}

static int handler_fault(struct kprobe *p,struct pt_regs *regs, int trapnr)
{
    printk(KERN_DEBUG "fault_handler: p->addr = 0x%p, trap #%dn"
                                        p->addr,trapnr);
    return 0;
}

static int __init kprobe_init(void)
{
    int ret;
    kp.pre_handler = handler_pre;                       //探测点的调用前
    kp.post_handler = handler_post;                     //探测点的调用后
    kp.fault_handler = handler_fault;
 
    ret = register_kprobe(&kp);  /*注册kprobe*/
    if (ret < 0) {
        printk(KERN_DEBUG "register_kprobe failed, returned %d\n", ret);
        return ret;
    }
    printk(KERN_DEBUG "Planted kprobe at %p\n", kp.addr);
    return 0;
}

static void __exit kprobe_exit(void)
{
    unregister_kprobe(&kp);
    printk(KERN_DEBUG "kprobe at %p unregistered\n", kp.addr);
}

module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");