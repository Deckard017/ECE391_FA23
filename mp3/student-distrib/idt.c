#include "idt.h"
#include "lib.h"
#include "linkage.h"
#include "x86_desc.h"

// initialize the IDT
void idt_init() {
    int i;
    //fill the features for all the descriptors in the idt
    for(i=0; i< NUM_VEC; i++)
    {
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].dpl=0;
        idt[i].reserved3=0;
        idt[i].reserved0 = 0;
        idt[i].size = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].present = 1;
        //set the features of the idt
        if(i<0x20)
        {
            idt[i].dpl=0;
            idt[i].reserved3=1;
            idt[i].reserved0 = 0;
            idt[i].size = 1;
            idt[i].reserved1 = 1;
            idt[i].reserved2 = 1;
            idt[i].present = 1;
        }
        //pit
        if(i==0x20)
        {
            idt[i].dpl=0;
            idt[i].reserved3=0;
            idt[i].reserved0 = 0;
            idt[i].size = 1;
            idt[i].reserved1 = 1;
            idt[i].reserved2 = 1;
            idt[i].present = 1;
        }
        //system call
        if(i==0x80)
        {
            idt[i].dpl=3;
            idt[i].reserved3=1;
            idt[i].reserved0 = 0;
            idt[i].size = 1;
            idt[i].reserved1 = 1;
            idt[i].reserved2 = 1;
            idt[i].present = 1;
        }
        //keyboarad
        if(i==0x21)
        {
            idt[i].dpl=0;
            idt[i].reserved3=0;
            idt[i].reserved0 = 0;
            idt[i].size = 1;
            idt[i].reserved1 = 1;
            idt[i].reserved2 = 1;
            idt[i].present = 1;
        }
        //rtc
        if(i==0x28)
        {
            idt[i].dpl=0;
            idt[i].reserved3=0;
            idt[i].reserved0 = 0;
            idt[i].size = 1;
            idt[i].reserved1 = 1;
            idt[i].reserved2 = 1;
            idt[i].present = 1;
        }
        

    }
    SET_IDT_ENTRY(idt[0], divide_error_linkage);
    SET_IDT_ENTRY(idt[1], debug_linkage);
    SET_IDT_ENTRY(idt[2], nmi_linkage);
    SET_IDT_ENTRY(idt[3], breakpoint_linkage);
    SET_IDT_ENTRY(idt[4], overflow_linkage);
    SET_IDT_ENTRY(idt[5], bound_range_exceeded_linkage);
    SET_IDT_ENTRY(idt[6], invalid_opcode_linkage);
    SET_IDT_ENTRY(idt[7], device_not_available_linkage);
    SET_IDT_ENTRY(idt[8], double_fault_linkage);
    SET_IDT_ENTRY(idt[9], coprocessor_segment_overrun_linkage);
    SET_IDT_ENTRY(idt[10], invalid_tss_linkage);
    SET_IDT_ENTRY(idt[11], segment_not_present_linkage);
    SET_IDT_ENTRY(idt[12], stack_segment_fault_linkage);
    SET_IDT_ENTRY(idt[13], general_protection_fault_linkage);
    SET_IDT_ENTRY(idt[14], page_fault_linkage);
    SET_IDT_ENTRY(idt[15], reserved_linkage);
    SET_IDT_ENTRY(idt[16], floating_point_error_linkage);
    SET_IDT_ENTRY(idt[17], alignment_check_linkage);
    SET_IDT_ENTRY(idt[18], machine_check_linkage);
    SET_IDT_ENTRY(idt[19], simd_floating_point_exception_linkage);
    SET_IDT_ENTRY(idt[0x80], system_call_linkage);
    SET_IDT_ENTRY(idt[0x20], pit_linkage);
    SET_IDT_ENTRY(idt[0x21], keyboard_linkage);
    SET_IDT_ENTRY(idt[0x28], rtc_linkage);
    lidt(idt_desc_ptr);     //load the idt
}




// Exception handlers
void handle_divide_error(){
    //cli();
    //clear();
    printf("Divide Error\n");
    while(1)
    {
        
    }

}
/*Function to handle exception*/
void handle_debug(){
    
    printf("Debug\n");
    while(1);
}

void handle_nmi_interrupt(){
    
    printf("NMI Interrupt\n");
    while(1);
}

void handle_breakpoint(){
    
    printf("Breakpoint\n");
    while(1);
}

void handle_overflow(){
    
    printf("Overflow\n");
    while(1);
}

void handle_bound_range_exceeded(){
    
    printf("Bound Range Exceeded\n");
    while(1);
}

void handle_invalid_opcode(){
    
    printf("Invalid Opcode\n");
    while(1);
}

void handle_device_not_available(){
    
    printf("Device Not Available\n");
    while(1);
}

void handle_double_fault(){
    
    printf("Double Fault\n");
    while(1);
}

void handle_coprocessor_segment_overrun(){
    
    printf("Coprocessor Segment Overrun\n");
    while(1);
}

void handle_invalid_tss(){
    
    printf("Invalid TSS\n");
    while(1);
}

void handle_segment_not_present(){
    
    printf("Segment Not Present\n");
    while(1);
}

void handle_stack_segment_fault(){
    
    printf("Stack Segment Fault\n");
    while(1);
}

void handle_general_protection_fault(){
    
    printf("General Protection\n");
    while(1);
}   



void handle_page_fault(){
    
    printf("Page Fault\n");
    while(1);
}

void handle_reserved(){
    
    printf("Reserved\n");
    while(1);
}

void handle_floating_point_error(){
    
    printf("Floating Point Error\n");
    while(1);
}

void handle_alignment_check(){
    
    printf("Alignment Check\n");
    while(1);
}

void handle_machine_check(){
    
    printf("Machine Check\n");
    while(1);
}

void handle_simd_floating_point_exception(){
    
    printf("SIMD Floating Point Exception\n");
    while(1);
}




//system call handler
void handle_system_call(){
    
    printf("System Call\n");
}





