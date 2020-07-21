#include <stdio.h>


// FIFO命名管道- 连接不关联的进程

/**********************************************
 * mkfifo(char* name, mode_t mode)
   指定的权限模式创建FIFO
 * unlink(fifoname)
   删除FIFO
 * open(fifoname, O_RDONLY | O_RDONLY)  
   阻塞进程直到某一进程打开FIFO写
 * read | write 系统调用读写
***********************************************/

