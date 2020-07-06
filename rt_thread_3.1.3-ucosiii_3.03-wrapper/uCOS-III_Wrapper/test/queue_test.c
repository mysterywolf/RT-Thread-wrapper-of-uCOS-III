#include <os.h>

#define THREAD_PRIORITY         5
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5


ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
static struct rt_thread thread2;

ALIGN(RT_ALIGN_SIZE)
static char thread3_stack[1024];
static struct rt_thread thread3;

static OS_Q DATA_Msg;				//定义一个消息队列，用于发送数据
#define DATAMSG_Q_NUM	4	//发送数据的消息队列的数量

static char buffer[128];

/* 线程2入口 */
static void thread2_entry(void *param)
{
    OS_ERR err;
    unsigned int i=0;
    while(1) 
    {
        i++;
        rt_sprintf(buffer,"test:%d",i);
        //发送消息
        OSQPost((OS_Q*		)&DATA_Msg,		
                (void*		)buffer,
                (OS_MSG_SIZE)rt_strlen(buffer),
                (OS_OPT		)OS_OPT_POST_FIFO,
                (OS_ERR*	)&err);    
        
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("queue post err:%d!\r\n",err);
        }            
                
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
    }
}

/* 线程3入口 */
static void thread3_entry(void *param)
{
	char  *p;
	OS_MSG_SIZE size;
	OS_ERR err; 
    
    while(1)
    {
		//请求消息
		p=OSQPend((OS_Q*		)&DATA_Msg,   
				  (OS_TICK		)0,
                  (OS_OPT		)OS_OPT_PEND_BLOCKING,
                  (OS_MSG_SIZE*	)&size,	
                  (CPU_TS*		)0,
                  (OS_ERR*		)&err);
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("queue pend err!:%d\r\n",err);
        }
        else
        {
            rt_kprintf("pended: str:%s,size:%d\r\n",p,size);
        }       
    }
}

void queue_test (void)
{
    OS_ERR err;
                   
	//创建消息队列DATA_Msg
	OSQCreate ((OS_Q*		)&DATA_Msg,	
                (CPU_CHAR*	)"DATA Msg",	
                (OS_MSG_QTY	)DATAMSG_Q_NUM,	
                (OS_ERR*	)&err);	  
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("queue create err!:%d\r\n",err);
    }    
    
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
