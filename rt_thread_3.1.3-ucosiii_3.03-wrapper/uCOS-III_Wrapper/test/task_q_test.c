#include <os.h>

#define THREAD_PRIORITY         5
#define THREAD_STACK_SIZE       256
#define THREAD_TIMESLICE        5

ALIGN(RT_ALIGN_SIZE)
static CPU_STK thread1_stack[THREAD_STACK_SIZE];
static OS_TCB thread1;

ALIGN(RT_ALIGN_SIZE)
static CPU_STK thread2_stack[THREAD_STACK_SIZE];
static OS_TCB thread2;

static char buffer[128];

/*测试：线程1向线程2使用内建消息队列发送消息*/

/* 线程1入口 */
static void thread1_entry(void *param)
{
    OS_ERR err;
    unsigned int i=0;
    while(1) 
    {
        i++;
        rt_sprintf(buffer,"test:%d",i);
        
        OSTaskQPost(&thread2,buffer,rt_strlen(buffer),OS_OPT_POST_FIFO,&err);
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("queue post err:%d!\r\n",err);
        }      
        
        OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);
    }
}
    

/* 线程2入口 */
static void thread2_entry(void *param)
{
	char  *p;
	OS_MSG_SIZE size;
    OS_ERR err;
              
    while(1)
    {
        p = OSTaskQPend(0,OS_OPT_PEND_BLOCKING,&size,0,&err);
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("queue pend err!:%d\r\n",err);
        }
        else
        {
            rt_kprintf("pended: str:%s,size:%d\r\n",p,size);
        }   

        /*测试嵌套挂起*/
        OSTaskSuspend(&thread1,&err);
        OSTaskSuspend(&thread1,&err);
        OSTaskSuspend(&thread1,&err);
        OSTaskSuspend(&thread1,&err);
        OSTaskSuspend(&thread1,&err);
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
        OSTaskResume(&thread1,&err);
        OSTaskResume(&thread1,&err);
        OSTaskResume(&thread1,&err);
        OSTaskResume(&thread1,&err);
        OSTaskResume(&thread1,&err);
    }
}
                 

void task_q_test (void)
{
    OS_ERR err;
        
	OSTaskCreate(&thread2,		            //任务控制块
			   (CPU_CHAR*)"thread2", 		//任务名字
               thread2_entry, 			    //任务函数
               0,					        //传递给任务函数的参数
               THREAD_PRIORITY-1,           //任务优先级
               thread2_stack,	            //任务堆栈基地址
               THREAD_STACK_SIZE/10,	    //任务堆栈深度限位
               THREAD_STACK_SIZE,		    //任务堆栈大小
               20,					        //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
               THREAD_TIMESLICE,			//当使能时间片轮转时的时间片长度，为0时为默认长度，
               0,					        //用户补充的存储区
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
               &err);
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("task create err:%d\r\n",err);
    }   
        
        
	OSTaskCreate(&thread1,		            //任务控制块
			   (CPU_CHAR*)"thread1", 		//任务名字
               thread1_entry, 			    //任务函数
               0,					        //传递给任务函数的参数
               THREAD_PRIORITY,           //任务优先级
               thread1_stack,	            //任务堆栈基地址
               THREAD_STACK_SIZE/10,	    //任务堆栈深度限位
               THREAD_STACK_SIZE,		    //任务堆栈大小
               5,					        //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
               THREAD_TIMESLICE,			//当使能时间片轮转时的时间片长度，为0时为默认长度，
               0,					        //用户补充的存储区
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
               &err);
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("task create err:%d\r\n",err);
    }    
    
}
