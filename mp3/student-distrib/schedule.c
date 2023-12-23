#include "schedule.h"
#include "page.h"
#include "x86_desc.h"
#include "lib.h"
#include "terminal.h"
#include "syscall.h"
#include "pit.h"

int32_t schedules[3]={TERMINAL_BEFORE_INITIALIZED,TERMINAL_BEFORE_INITIALIZED,TERMINAL_BEFORE_INITIALIZED};
int32_t curr_schedule=-1;
/*
* The helper function to use schedule
*/
void schedule_init()
{
    helper_function_of_save_ebp();
    helper_function_of_next_sche();

    vidmem_remap(curr_schedule);
    helper_function_of_set_tss();
}

/*
* The helper function to save current ebp
*/
void helper_function_of_save_ebp()
{
    pcb_t* curr_pcb=helper_function_of_get_curr_pcb();
    register uint32_t ebp_saved asm("ebp");
    curr_pcb->ebp_for_schedule=ebp_saved;
}

/*
* To check the next schedule
*/
void helper_function_of_next_sche()
{
    uint32_t index_of_one_pid=0x08000000 >> 22;//index of the page directory entry
    curr_schedule=(curr_schedule+1)%3;
    if(schedules[(int32_t)curr_schedule]==TERMINAL_BEFORE_INITIALIZED)
    {
        vidmem_remap(curr_schedule);
        execute((uint8_t*)"shell");
        //putc_user('a');
    }
    int32_t temp=schedules[(int32_t)curr_schedule];
    set_PDE_mb(index_of_one_pid,1,1,1,0,2+temp);//set the page directory, v128mb -> p8mb
    asm volatile(
                "movl %0, %%eax \n\t"
                "movl %%eax, %%cr3   \n\t"
                :
                :"r"(&Page_Dir)
                :"%eax"
                );
}
/*
* The helper function to set the tss
*/
void helper_function_of_set_tss()
{
    tss.ss0=KERNEL_DS;
    int32_t nextpidpid=schedules[(int32_t)curr_schedule];  
    pcb_t* temp=(pcb_t*)(0x800000-nextpidpid*0x2000);
    tss.esp0=(uint32_t) temp-4;
    pcb_t* next_pcb=(pcb_t*)(0x800000-(nextpidpid+1)*0x2000);
    asm volatile(
               "movl %0, %%ebp \n\
                leave          \n\
                ret            \n"
            :  /* no output */
            :  "r" (next_pcb->ebp_for_schedule) \
            :  "ebp");
}
