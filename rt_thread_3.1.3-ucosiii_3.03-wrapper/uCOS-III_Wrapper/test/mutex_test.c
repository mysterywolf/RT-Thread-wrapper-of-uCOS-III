#include <os.h>

#define THREAD_PRIORITY         5
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static OS_MUTEX	SYNC_SEM;		//定义一个信号量，用于任务同步

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
static struct rt_thread thread2;

ALIGN(RT_ALIGN_SIZE)
static char thread3_stack[1024];
static struct rt_thread thread3;


/* 线程2入口 */
static void thread2_entry(void *param)
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
                       
    while(1)
    {
        OSMutexPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
        if(err != OS_ERR_NONE)
        {
            rt_kprintf("thread2 mutex err:%d\r\n",err);
        }
        rt_kprintf("thread2 has pended a mutex\r\n");
        OSTimeDlyHMSM(0,0,0,1000,OS_OPT_TIME_PERIODIC,&err);
        OSMutexPost(&SYNC_SEM,OS_OPT_POST_NONE,&err);//发送信号量
        //OSMutexDel(&SYNC_SEM,OS_OPT_DEL_ALWAYS,&err);        
    }
}

/* 线程3入口 */
static void thread3_entry(void *param)
{
    OS_ERR err;
    while(1)
    {
        OSMutexPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
        if(err != OS_ERR_NONE)
        {
            rt_kprintf("thread3 mutex err:%d\r\n",err);
        }
        rt_kprintf("thread3 has pended a mutex\r\n");
        
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
        OSMutexPost(&SYNC_SEM,OS_OPT_POST_NONE,&err);//发送信号量
    }
}

void mutex_test (void)
{
    rt_thread_init(&thread2,
                   "thread2",
                   thread2_entry,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),
                   THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    rt_thread_startup(&thread2);
                   
    rt_thread_init(&thread3,
                   "thread3",
                   thread3_entry,
                   RT_NULL,
                   &thread3_stack[0],
                   sizeof(thread3_stack),
                   THREAD_PRIORITY, THREAD_TIMESLICE);
    rt_thread_startup(&thread3);                   
                   
}
