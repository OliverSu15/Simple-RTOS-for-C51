#include <REG52.H>

#define MAX_TASK 4 //最大任务数，看实际需求改动，且要留出空间给idle任务
#define MAX_STACK_DEPTH 24 //私栈的深度，看实际需求改动
#define UPDATE_TIME 1000

//因为Keil通过bit实现bool，而bit又不让当struct的成员，所以粗略实现了一个bool类型
typedef enum{
    false = 0,  
    true  = 1,  
}bool;  

//任务的结构
typedef struct
{
    unsigned char delay;//延迟的时间
    unsigned char stack[MAX_STACK_DEPTH];//私栈
    unsigned char sp;//栈顶指针
    bool finished;//任务是否完成的指示
} task;

extern unsigned char os_idle_task[MAX_STACK_DEPTH];//idle任务的私栈
extern task idata os_task[MAX_TASK]; //所有任务的数组

//任务切换，不要直接使用
//请使用os_delay_ms、os_continue、os_wait、os_start_over来切换任务
void os_switch();

//idle任务，不断调用os_switch来切换任务
//防止所有任务都在进行，逻辑回不到os_switch的错误
void os_idle();

//装载函数到私栈中
//fn_addr：函数的地址，直接把函数名填进来就好，请注意不要在函数名后面加（）
//task_id
void os_load_task(unsigned int fn_addr, unsigned char task_id);

//初始换函数，只要在os_start调用前调用即可
void os_init();

//延时切换函数：记录延时，先将任务切换出去，等延时结束再切换回来
//delay_time：要延时的时间
//task_id：调用该函数的任务的id
void os_delay_ms(unsigned char delay_time, unsigned char task_id);

//完成切换函数：将任务标记完成，请求任务切换
//task_id：调用该函数的任务的id
void os_continue(unsigned char task_id);

//检测所有序号小于task_id的任务是否以完成
//task_id：一般为调用该函数的任务的id，其他情况也可以，只是要注意task_id不在检测的范围内
//都完成了返回true，否则为false
bool os_every_task_before_finished(unsigned char task_id);

//出于某种原因，既不将任务标记为完成，也没有延时的需求，但请求任务切换
//task_id：调用该函数的任务的id
void os_wait(unsigned char task_id);

//重置全部任务，重启循环。
//task_id：一般为os_start时填入的参数
void os_start_over(unsigned char task_id);

//启动系统
//task_id：要作为第一个启动的任务的id，建议为0
void os_start(unsigned char task_id);