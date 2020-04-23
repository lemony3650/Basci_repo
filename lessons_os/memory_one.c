#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>

#define PROCESS_NAME_LEN  32		//进程名长度	
#define MIN_SLICE	  10			//最小碎片大小
#define DEFAULT_MEM_SIZE  1024		//内存大小
#define DEFAULT_MEM_START 0			//起始位置

//内存分配算法
#define MA_FF 1
#define MA_BF 2
#define MA_WF 3

/*************************************************************************************/
//空闲块的数据结构
typedef struct _free_block_type{
	int size;
	int start_addr;
	struct _free_block_type *next;
}free_block_type;
//进程已分配到的内存块
typedef struct _allocated_block{
	int pid;
	int size;
	int start_addr;
	char process_name[PROCESS_NAME_LEN];
	struct _allocated_block *next;
}allocated_block;
/*************************************************************************************/
free_block_type *free_block;						//指向内存中空闲块链表的首指针
allocated_block *allocated_block_head = NULL;		//进程分配内存块链表首指针

int mem_size = DEFAULT_MEM_SIZE;			//内存大小
int ma_algorithm = MA_FF;					//当前分配算法
static int pid = 0;							//初始pid
int flag =  0;								//设置内存大小标志

free_block_type* init_free_block(int mem_size);		//初始化空闲区
void display_menu();					//显示模块
int  set_mem_size();					//设置内存大小
void set_algorithm();					//设置当前分配算法
void rearrange(int algorithm);
void rearrange_FF();
void rearrange_BF();
void rearrange_WF();
int  new_process();
int allocate_mem(allocated_block *ab);
void kill_process();
allocated_block *find_process(int pid);
int free_mem(allocated_block *ab);
int dispose(allocated_block *free_ab);
int display_mem_usage();
void do_exit();

int main()
{
	char choice;
	free_block = init_free_block(mem_size);		//初始化空闲区
	while(1)
	{
		display_menu();				//显示菜单
		fflush(stdin);				//清空输入缓冲区
		choice = getchar();
		switch(choice)
		{
			case '1':set_mem_size();				break;
			case '2':set_algorithm();    flag = 1;  break;	//设置算法
			case '3':new_process();      flag = 1;  break;
			case '4':kill_process();     flag = 1;  break;
			case '5':display_mem_usage();flag = 1;  break;	//显示内存使用		
			case '0':do_exit();			 exit(0);
			default :break;
		}
		while(!getchar());
	}
}
//初始化空闲块，默认为一块，可指定大小及起始地址
free_block_type* init_free_block(int mem_size){

	free_block_type *fb;	//free_block
	fb = (free_block_type*)malloc(sizeof(free_block_type));	//fb申请地址空间
	if(fb == NULL){
		printf("No mem\n");
		return NULL;
	}
	fb->size 	= mem_size;
	fb->start_addr  = DEFAULT_MEM_START;
	fb->next 	= NULL;
	return fb;
}
//显示模块
void display_menu(){

	printf("\n");
	printf("----------------------------------------------\n");
	printf("1 - Set memory size (default = %d)\n",DEFAULT_MEM_SIZE);
	printf("2 - Select memory allocation algorithm\n");
	printf("3 - New process \n");
	printf("4 - Terminate a process \n");
	printf("5 - Display memory usage \n");
	printf("0 - Exit\n");
	printf("----------------------------------------------\n");
}
//设置内存大小
int set_mem_size(){

	int size;
	if(flag != 0){//防止重复设置
		printf("Cannot set memory size again\n");
		return 0;
	}
	printf("Total memory size =");
	scanf("%d",&size);
	if(size > 0){
		mem_size 	 = size;
		free_block->size = mem_size;
	}
	flag = 1;
	return 1;
}

//设置当前分配算法
void set_algorithm(){
	
	int algorithm;
	printf("\t1 - First Fit\n");
	printf("\t2 - Best  Fit\n");
	printf("\t3 - Worst Fit\n");
	scanf("%d",&algorithm);

	if(algorithm >= 1 && algorithm <= 3)
		ma_algorithm = algorithm;
	rearrange(ma_algorithm);
}
//按指定算法整理内存空闲块链表
void rearrange(int algorithm){

	switch(algorithm){
	case MA_FF: rearrange_FF();	break;
	case MA_BF: rearrange_BF();	break;
	case MA_WF: rearrange_WF();	break;
	}
}

void rearrange_FF(){
	free_block_type *fp,*rear;
	int size;
	int start_addr;
	for(fp=free_block;fp->next!=NULL;fp=fp->next)
	for(rear=fp->next;rear!=NULL;rear=rear->next){
		if(fp->start_addr > rear->start_addr){
			start_addr		 = fp->start_addr;
			fp->start_addr	 = rear->start_addr;
			rear->start_addr = start_addr;
			size			 = fp->size;
			fp->size		 = rear->size;
			rear->size		 = size;
		}
	}
}
void rearrange_BF(){
	free_block_type *fp,*rear;
	int size;
	int start_addr;
	for(fp=free_block;fp->next!=NULL;fp=fp->next)
	for(rear=fp->next;rear!=NULL;rear=rear->next){
		if(fp->size > rear->size){
			size			 = fp->size;
			fp->size		 = rear->size;
			rear->size		 = size;
			start_addr		 = fp->start_addr;
			fp->start_addr	 = rear->start_addr;
			rear->start_addr = start_addr;
		}
	}
	
}
void rearrange_WF(){
	free_block_type *fp,*rear;
	int size;
	int start_addr;
	for(fp=free_block;fp->next!=NULL;fp=fp->next)
	for(rear=fp->next;rear!=NULL;rear=rear->next){
		if(fp->size < rear->size){
			size			 = fp->size;
			fp->size		 = rear->size;
			rear->size		 = size;
			start_addr		 = fp->start_addr;
			fp->start_addr	 = rear->start_addr;
			rear->start_addr = start_addr;
		}
	}

}
//创建新进程
int new_process(){

	allocated_block *ab;
	int size;
	int ret;

	ab = (allocated_block*)malloc(sizeof(allocated_block));
	if(!ab)exit(-5);

	ab->next = NULL;	pid++;
	sprintf(ab->process_name,"PROCESS-%02d",pid);
	ab->pid = pid;
	printf("Memory for %s:",ab->process_name);
	scanf("%d",&size);
	if(size > 0)ab->size = size;

	ret = allocate_mem(ab);
	//从空闲区分配内存，1表OK
	if((ret == 1)&&(allocated_block_head == NULL)){//若head未赋值，则赋值
		allocated_block_head = ab;
		return 1;
	}
	else if(ret == 1){//分配成功，插入分配链表
		ab->next = allocated_block_head;
		allocated_block_head = ab;
		return 2;
	}
	else if(ret == -1){//分配不成功
		printf("Allocation fail\n");
		free(ab);
		return -1;
	}

	return 3;
}
//分配内存模块
int allocate_mem(allocated_block *ab)
{
	free_block_type *fbt,*pre;
	int request_size = ab->size;
	fbt = pre = free_block;
	fbt = free_block->next;

	rearrange_WF();
	if((pre->size - request_size) <= MIN_SLICE && (request_size < pre->size))
	{
		ab->size			   = pre->size;
		free_block->size	   = free_block->size - ab->size;
		ab->start_addr		   = pre->start_addr;
		free_block->start_addr = free_block->start_addr + ab->size;
		free_block			   = pre->next;
		free(pre);
		return 1;
	}else if((pre->size-request_size) > MIN_SLICE && (request_size < pre->size))
	{
		ab->size			   = request_size;
		free_block->size	   = free_block->size - ab->size;
		ab->start_addr		   = pre->start_addr;
		free_block->start_addr = free_block->start_addr + ab->size;
		return 1;
	
	}else if(pre->size < request_size){
		while((pre->size < request_size)&&(fbt!=NULL))
		{
			pre->size = pre->size + fbt->size;
			pre->next = fbt->next;
			free(fbt);
			fbt = pre->next;

			if((pre->size - request_size) <= MIN_SLICE && (request_size < pre->size))
			{
				ab->size			   = pre->size;
				free_block->size	   = free_block->size - ab->size;
				ab->start_addr		   = pre->start_addr;
				pre->start_addr		   = free_block->start_addr + ab->size;
				free_block			   = pre->next;
				free(pre);
				return 1;
			}else if((pre->size - request_size) > MIN_SLICE && (request_size < pre->size))
			{
				ab->size			   = request_size;
				free_block->size	   = free_block->size - ab->size;
				ab->start_addr		   = pre->start_addr;
				pre->start_addr		   = free_block->start_addr + ab->size;
				return 1;
			}
		}
	}
	rearrange_WF();
	return 0;
}
//寻找进程
allocated_block* find_process(int pid)
{
	allocated_block *ab,*pre;
	pre = allocated_block_head;
	ab  = allocated_block_head->next;
	while(pre!=NULL)
	{
		if(pid == pre->pid)return pre;
		pre = ab;
		ab = ab->next;
	}
	return NULL;
}
void merge()
{
	free_block_type *rear,*pre;
	pre  = free_block;
	rear = free_block->next;
	while(rear!=NULL)
	{
		if(pre->start_addr + pre->size == rear->start_addr)
		{
			pre->size = pre->size + rear->size;
			pre->next = rear->next;
			free(rear);
			rear = pre->next;
		}else
		{
			pre  = pre->next;
			rear = rear->next;
		}
	}
	rearrange_WF();
}
//ab所表示的分期归还，并进行可能合并
int free_mem(allocated_block *ab){

//	int algorithm = ma_algorithm;

	free_block_type *fbt,*pre;
	pre = free_block;

	fbt = (free_block_type*)malloc(sizeof(free_block_type));
	if(!fbt)return -1;
	fbt->size = ab->size;
	fbt->start_addr = ab->start_addr;
	fbt->next = NULL;
	while(pre->next!=NULL)
		pre = pre->next;
	pre->next = fbt;
	rearrange_FF();
	merge();//合并内存
	return 1;
}
//释放ab数据结构结点
int dispose(allocated_block *free_ab)
{
	allocated_block *pre,*ab;
	if(free_ab == allocated_block_head)
	{	//如果释放第一个节点
		allocated_block_head = allocated_block_head->next;
		free(free_ab);
		return 1;
	}
	pre = allocated_block_head;
	ab  = allocated_block_head->next;
	while(ab != free_ab)
	{
		pre = ab;	
		ab  = ab->next;
	}
	pre->next = ab->next;
	free(ab);
	return 2;
}
//删除进程
void kill_process(){

	allocated_block *ab;
	int pid;
	printf("Kill Process,pid=");
	scanf("%d",&pid);
	ab = find_process(pid);
	if(ab != NULL){
		printf("find the process.\n");
		free_mem(ab);		//释放ab所表示的分配区
		dispose(ab);		//释放ab数据结构节点
	}else
		printf("no this\n");
}
//显示已分配分区
int display_mem_usage()
{
	free_block_type *fbt = free_block;
	allocated_block *ab  = allocated_block_head;

	if(fbt == NULL)return -1;
	printf("--------------------------------------------------------------\n");
	//显示空闲区
	printf("Free Memory:\n");
	printf("%20s %20s\n","strat_addr","size");
	while(fbt != NULL)
	{
		printf("%20d %20d\n",fbt->start_addr,fbt->size);
		fbt = fbt->next;
	}
	//显示分配区
	printf("\nUsed Memory:\n");
	printf("%10s %20s %10s %10s\n","PID","ProcessName","start_addr","size");
	while(ab != NULL)
	{
		printf("%10d %20s %10d %10d\n",ab->pid,ab->process_name,ab->start_addr,ab->size);
		ab = ab->next;
	}
	printf("---------------------------------------------------------------\n");
	return 0;
}
void do_exit()
{
	//定义两个指针逐个释放
	allocated_block *rear,*pre;			
	rear = pre = allocated_block_head;
	while(pre!=NULL)
	{
		rear = pre->next;
		free(pre);
		pre = rear;
	}
	allocated_block_head   = NULL;
	free_block->size	   = mem_size;
	free_block->start_addr = DEFAULT_MEM_SIZE;
	printf("free the memory is successful\n");
}

