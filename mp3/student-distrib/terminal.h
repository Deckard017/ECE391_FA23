#ifndef TERMINAL_H
#define TERMINAL_H

#include "types.h"

#define keyborad_buff_size 128

typedef struct terminal_t
{
    int cursor_x;   //cursor position
    int cursor_y;
    uint8_t keyboard_buffer[keyborad_buff_size];    //keyboard buffer (temporary storage)
    volatile int flag_for_read; //flag for read
    uint32_t count; //character count for the buffer
    uint8_t* vidmem_addr;   //video memory address
} terminal_t;

extern void switch_terminal(uint8_t terminal_num);
extern int32_t terminal_init();
extern int32_t terminal_open(const uint8_t* filename);
extern int32_t terminal_close(int32_t fd);
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
extern terminal_t* get_terminal();
extern int32_t update_cursor(int32_t x, int32_t y);
extern int32_t move_cursor_right();
extern int32_t move_cursor_left();
extern void print_cursor();
extern void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
extern void disable_cursor();
extern int32_t scroll_up();
extern volatile uint8_t cur_terminal;
extern terminal_t terminals[3];
extern uint8_t* terminal1_vidaddr;
extern uint8_t* terminal2_vidaddr;
extern uint8_t* terminal3_vidaddr;
#endif
