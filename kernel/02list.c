#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

#define N 10

struct _numlist {
	int num;
	struct list_head list;
};

struct _numlist head_node;		//定义头结点 

static int __init mylist_init(void)
{
	struct _numlist *node;		//用于承接申请节点 
	struct _numlist *ptr;		//遍历
	struct list_head *pos;
	int i;

	printk("list is starting...\n");
	INIT_LIST_HEAD(&head_node.list);	//初始化 指针域指向自身

	for(i = 0;i < N;i++){
		node = (struct _numlist*)kmalloc(sizeof(struct _numlist),GFP_KERNEL);
		node->num = i+1;				//数据域改变

		list_add_tail(&node->list,&head_node.list);		//struct list_head *new , *headnode
		printk("Node %d has added to list...\n",i+1);
	}

	i = 1;
	list_for_each(pos,&head_node.list){
		ptr = list_entry(pos,struct _numlist,list);		//返回的是struct 类型
		printk("Node %d is data： %d\n",i,ptr->num);
		i++;
	}

	return 0;
}

static void __exit mylist_exit(void)
{
	struct list_head *pos,*n;
	struct _numlist *p;
	int i = 1;

	list_for_each_safe(pos,n,&head_node.list){
		list_del(pos);
		p = list_entry(pos,struct _numlist,list);
		kfree(p);
		printk("Node %d has removed form the list...\n",i++);
	}
	printk("list is exiting...\n");
}

module_init(mylist_init);
module_exit(mylist_exit);




