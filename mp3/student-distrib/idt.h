#ifndef IDT_H
#define IDT_H

// initialize the IDT
extern void idt_init();
// Exception handlers
extern void handle_divide_error();
extern void handle_debug();
extern void handle_nmi_interrupt();
extern void handle_breakpoint();
extern void handle_overflow();
extern void handle_bound_range_exceeded();
extern void handle_invalid_opcode();
extern void handle_device_not_available();
extern void handle_double_fault();
extern void handle_coprocessor_segment_overrun();
extern void handle_invalid_tss();
extern void handle_segment_not_present();
extern void handle_stack_segment_fault();
extern void handle_general_protection_fault();
extern void handle_page_fault();
extern void handle_reserved();
extern void handle_x87_floating_point_exception();
extern void handle_alignment_check();
extern void handle_machine_check();
extern void handle_simd_floating_point_exception();
// System call handler
extern void handle_system_call();





#endif
