#include <os.h>


#define THREAD_PRIORITY         5
#define THREAD_STACK_SIZE       256
#define THREAD_TIMESLICE        50

static OS_MUTEX	SYNC_SEM;		//定义一个信号量，用于任务同步

ALIGN(RT_ALIGN_SIZE)
static CPU_STK thread2_stack[THREAD_STACK_SIZE];
static OS_TCB thread2;

ALIGN(RT_ALIGN_SIZE)
static CPU_STK thread3_stack[THREAD_STACK_SIZE];
static OS_TCB thread3;


/* 线程2入口 */
static void thread2_entry(void *param)
{
    OS_ERR err;
    OS_OBJ_QTY pending_mutex;
               
    while(1)
    {
        OSMutexPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
        OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);
        pending_mutex = OSMutexDel(&SYNC_SEM,OS_OPT_DEL_NO_PEND,&err);  
        if(err == OS_ERR_TASK_WAITING)
        {
            rt_kprintf("还有%d个任务正在等待该信号量，无法删除\r\n",pending_mutex);
            OSMutexPendAbort(&SYNC_SEM,OS_OPT_PEND_ABORT_1|OS_OPT_POST_NO_SCHED,&err);
            OSMutexDel(&SYNC_SEM,OS_OPT_DEL_NO_PEND,&err);
            if(err==OS_ERR_NONE)
            {
                rt_kprintf("删除成功\r\n");
            }
        }
        else if (err == OS_ERR_NONE)
        {
            rt_kprintf("信号量删除成功\r\n");
        }
        else
        {
            rt_kprintf("删除失败err:%d\r\n",err);
        }        
    }
}

/* 线程3入口 */
static void thread3_entry(void *param)
{
    OS_ERR err;
    while(1)
    {
        OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);
        OSMutexPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
        if(err == OS_ERR_PEND_ABORT)
        {
            rt_kprintf("thread3 mutex ARBORT");
        }
        else if (err == OS_ERR_NONE)
        {
            rt_kprintf("thread3 has pended a mutex\r\n");
        }
        while(1)
        {
            OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_PERIODIC,&err);
        }
    }
}

void mutex_test (void)
{
    OS_ERR err;
 	//创建一个互斥量
	OSMutexCreate ((OS_MUTEX*)&SYNC_SEM,
                 (CPU_CHAR*	)"SYNC_SEM",	
                 (OS_ERR*	)&err);    
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("create err!\r\n");
    }
               
 	OSTaskCreate(&thread3,		            //任务控制块
			   (CPU_CHAR*)"thread3", 		//任务名字
               thread3_entry, 			    //任务函数
               0,					        //传递给任务函数的参数
               THREAD_PRIORITY  ,           //任务优先级
               thread3_stack,	            //任务堆栈基地址
               THREAD_STACK_SIZE/10,	    //任务堆栈深度限位
               THREAD_STACK_SIZE,		    //任务堆栈大小
               0,					        //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
               THREAD_TIMESLICE,			//当使能时间片轮转时的时间片长度，为0时为默认长度，
               0,					        //用户补充的存储区
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
               &err);
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("task create err:%d\r\n",err);
    }  

	OSTaskCreate(&thread2,		            //任务控制块
			   (CPU_CHAR*)"thread2", 		//任务名字
               thread2_entry, 			    //任务函数
               0,					        //传递给任务函数的参数
               THREAD_PRIORITY-1,           //任务优先级
               thread2_stack,	            //任务堆栈基地址
               THREAD_STACK_SIZE/10,	    //任务堆栈深度限位
               THREAD_STACK_SIZE,		    //任务堆栈大小
               0,					        //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
               THREAD_TIMESLICE,			//当使能时间片轮转时的时间片长度，为0时为默认长度，
               0,					        //用户补充的存储区
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
               &err);
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("task create err:%d\r\n",err);
    } 
        
}
