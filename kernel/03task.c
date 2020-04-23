#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/init_task.h>

MODULE_LICENSE("GPL");

static int print_pid(void)
{
	struct task_struct *task,*p;
	struct list_head *pos;

	int count = 0;
	printk("Hello World enter begin: \n");
	task = &init_task;
	list_for_each(pos,&task->tasks)		//	struct list_head tasks;
	{
		p = list_entry(pos ,struct task_struct ,tasks);
		count++;
		printk("%d ---> %s\n",p->pid,p->comm);
		printk("state: %ld  -----> parent: %d -----> sub_parent: %d\n",p->state,(p->parent)->pid,(p->real_parent)->pid);
		
	}
	printk("the number of process is: %d\n",count);
	
	return 0;
}
static void print_cleanup(void)
{
	printk("Goodbye,world...\n");
}

module_init(print_pid);
module_exit(print_cleanup);
