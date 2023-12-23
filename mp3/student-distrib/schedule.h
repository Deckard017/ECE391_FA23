#ifndef  _SCHEDULE_H
#define  _SCHEDULE_H

#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "syscall.h"
#include "page.h"
#include "x86_desc.h"
#include "terminal.h"
#include "rtc.h"
#include "pit.h"
#include "keyboard.h"
#include "filesystem.h"
#define TERMINAL_BEFORE_INITIALIZED -2
#define TERMINAL_TERMINATED -1
void schedule_init();
void helper_function_of_save_ebp();
void helper_function_of_next_sche();
void helper_function_of_set_tss();

extern int32_t schedules[3];
extern int32_t curr_schedule;
#endif
