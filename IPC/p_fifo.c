#include <stdio.h>


// FIFO�����ܵ�- ���Ӳ������Ľ���

/**********************************************
 * mkfifo(char* name, mode_t mode)
   ָ����Ȩ��ģʽ����FIFO
 * unlink(fifoname)
   ɾ��FIFO
 * open(fifoname, O_RDONLY | O_RDONLY)  
   ��������ֱ��ĳһ���̴�FIFOд
 * read | write ϵͳ���ö�д
***********************************************/

