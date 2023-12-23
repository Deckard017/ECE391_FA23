#ifndef LINKAGE_H
#define LINKAGE_H

#include "types.h"
#include "rtc.h"
#include "keyboard.h"
#include "lib.h"
#include "idt.h"
#include "x86_desc.h"
#include "pit.h"


/*
The linkages the interrupt handlers use to call the C functions
*/

extern void rtc_linkage();
extern void keyboard_linkage();
extern void system_call_linkage();
extern void divide_error_linkage();
extern void debug_linkage();
extern void nmi_linkage();
extern void breakpoint_linkage();
extern void overflow_linkage();
extern void bound_range_exceeded_linkage();
extern void invalid_opcode_linkage();
extern void device_not_available_linkage();
extern void double_fault_linkage();
extern void coprocessor_segment_overrun_linkage();
extern void invalid_tss_linkage();
extern void segment_not_present_linkage();
extern void stack_segment_fault_linkage();
extern void general_protection_fault_linkage();
extern void page_fault_linkage();
extern void reserved_linkage();
extern void floating_point_error_linkage();
extern void alignment_check_linkage();
extern void machine_check_linkage();
extern void simd_floating_point_exception_linkage();
extern void pit_linkage();

#endif
