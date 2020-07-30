#include "os.h"

unsigned char os_idle_task[MAX_STACK_DEPTH];
task idata os_task[MAX_TASK];

void os_switch(){
    unsigned char i = 0;
    while (i < MAX_TASK)
    {
        //寻找优先度高且延时结束且未完成的任务
        if( !os_task[i].finished && os_task[i].delay == 0){
            SP = os_task[i].sp;//开始执行这个任务
            break;
        }
        i++;
    }
}

void os_idle(){
    while (1)
    {
        os_switch();
    }
}

void os_load_task(unsigned int fn_addr, unsigned char task_id){
    //初始化
    os_task[task_id].delay = 0;
    os_task[task_id].finished = false;
    //让内部的SP指向私栈
    os_task[task_id].sp = os_task[task_id].stack + 1;
    //装载函数地址
    os_task[task_id].stack[0] = (unsigned int) fn_addr & 0xff;//填充函数的低位地址
    os_task[task_id].stack[1] = (unsigned int) fn_addr >> 8;//充函数的高位地址
}

void os_init(){
    //初始化定时器
    EA = 1;
    IT0 = 1;
    TMOD = 0x01;
    ET0 = 1;  
    TH0 = (65536 - UPDATE_TIME)/256;
    TL0 = (65536 - UPDATE_TIME)%256;

    //将idle装载进一个系统拥有的私栈中
    os_idle_task[0] = (unsigned int) os_idle & 0xff;
    os_idle_task[1] = (unsigned int) os_idle >> 8;
}

void os_delay_ms(unsigned char delay_time, unsigned char task_id){
    TR0 = 0;//防止更新时发生中断

    os_task[task_id].delay += delay_time;
    os_task[task_id].sp = SP;//保存状态
    SP = os_idle_task + 1;//切换新的任务
    
    TR0 = 1;
}

void os_continue(unsigned char task_id){
    os_task[task_id].finished = true;
    os_task[task_id].sp = SP;
    SP = os_idle_task + 1;
}

bool os_every_task_before_finished(unsigned char task_id){
    unsigned int i = 0;
    while(i < task_id){
        if(!os_task[i].finished) return false;
        i++;
    }
    return true;
}

void os_wait(unsigned char task_id){
    os_task[task_id].sp = SP;
    SP = os_idle_task + 1;
}

void os_start_over(unsigned char task_id){
    unsigned char i = 0;
    while(i < MAX_TASK){
        os_task[i].finished = false;
        os_task[i].delay = 0;
        os_task[task_id].sp = os_task[task_id].stack + 1;
        i++;
    }
    SP = os_task[task_id].sp;
}

void os_start(unsigned char task_id) {
    SP = os_task[task_id].sp;
    TR0 = 1;
}

//这个中断的实现并不立即切换任务，请按需更改中断的实现
void os_time_update() interrupt 1 {
    unsigned char i = 0;
    //1MS定时，看需改动
    TH0 = (65536 - UPDATE_TIME)/256;
    TL0 = (65536 - UPDATE_TIME)%256;
    while(i < MAX_TASK){
        if(os_task[i].delay)
            os_task[i].delay--;
        i++;
    }
}