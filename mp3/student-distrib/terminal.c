#include "terminal.h"
#include "lib.h"
#include "keyboard.h"
#include "page.h"
#include "x86_desc.h"
#include "syscall.h"
#include "pit.h"
#include "schedule.h"
#define NUM_COLS    80
#define NUM_ROWS    25

uint8_t* terminal1_vidaddr=(uint8_t*) 0xB9000;
uint8_t* terminal2_vidaddr=(uint8_t*) 0xBA000;
uint8_t* terminal3_vidaddr=(uint8_t*) 0xBB000;
terminal_t terminals[3];
volatile uint8_t cur_terminal;
/*
interface for this function
input: none
output: 0
effect: initialize the terminal
*/
int32_t terminal_init()
{
    int i=0;
    for(i=0;i<3;i++)
    {
        // terminals[i].cursor_x=0;
        // terminals[i].cursor_y=22;
        terminals[i].flag_for_read=0;
        terminals[i].count=0;
        enable_cursor(0,14);
        print_cursor();
        //cur_terminal++;
        //execute((uint8_t*)"shell");
    }
    // terminals[0].vidmem_addr=(uint8_t*)terminal1_vidaddr;
    // terminals[1].vidmem_addr=(uint8_t*)terminal2_vidaddr;
    // terminals[2].vidmem_addr=(uint8_t*)terminal3_vidaddr;
    // terminal.cursor_x = get_screen_x(); //get the current cursor position
    // terminal.cursor_y = get_screen_y();
    // terminal.flag_for_read = 0; //set the flag for read
    //print_cursor();
    cur_terminal=0;
    return 0;
}
/*
interfacee for this function
input: filename
output: 0
effect: open the terminal
*/
int32_t terminal_open(const uint8_t* filename) 
{

    return 0;
}
/*
interface for this function
input: fd
output: 0
effect: close the terminal
*/
int32_t terminal_close(int32_t fd) 
{
    terminals[curr_schedule].count = 0; //reset the count
    return 0;
}

/*
interface for this function
input: none
output: terminal
effect: get the terminal
*/
terminal_t* get_terminal()
{
    return &(terminals[cur_terminal]);
}

/*
interface for this function
input: fd, buf, nbytes
output: i
effect: write to the terminal
*/
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) 
{
    int nbytes_temp;
    if(buf==NULL)
    {
        return 0;
    }
    if(nbytes<=0)
    {
        return 0;
    }
   //uint8_t* temp_buf = (uint8_t*)buf;
    for(nbytes_temp=0;nbytes_temp<nbytes &&  ((char*) buf)[nbytes_temp]!='\0';nbytes_temp++)
    {
        putc(((char*) buf)[nbytes_temp]);  //print the buffer
    }
    // for(i=0;i<nbytes;i++)
    // {
    //     ((char* )buf)[i]='\0';   //clear the buffer
    // }
    //putc('\n'); //print a new line after the command
    return nbytes_temp;
}
/*
interface for this function
input: fd, buf, nbytes
output: i
effect: read from the terminal
*/
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) 
{
    int32_t nbytes_temp;
    if(buf==NULL)
    {
        return 0;
    }
    if(nbytes<=0)
    {
        return 0;
    }
    memset(terminals[curr_schedule].keyboard_buffer, 0, keyborad_buff_size); //clear the buffer
    // for(i=0;i<nbytes;i++)
    // {
    //     ((uint8_t*)buf)[i]='\0';    //clear the buffer
    // }
    terminals[curr_schedule].count=0; 
    terminals[curr_schedule].flag_for_read=0;
    while(terminals[curr_schedule].flag_for_read==0);   //wait for the flag for read
    for(nbytes_temp=0;nbytes_temp<nbytes && nbytes_temp<keyborad_buff_size && terminals[curr_schedule].keyboard_buffer[nbytes_temp]!='\0';nbytes_temp++)
    {
        // if(terminal.keyboard_buffer[i]=='\n')
        // {
        //     break;
        // }
        ((char *)buf)[nbytes_temp]=terminals[curr_schedule].keyboard_buffer[nbytes_temp];   //copy the buffer
    }
   // ((char *)buf)[nbytes_temp++]= '\n'; //add a new line
   // ((uint8_t*)buf)[i]='\0'; //add a null terminator
    // int32_t j;
    // for(j=0;j<=i;j++)
    // {
    //     terminal.keyboard_buffer[j]='\0';   //clear the buffer
    // }
    // terminal.count=0;
    return nbytes_temp;
}
/*
interface for this function
input: x, y
output: 0
effect: update the cursor
*/
int32_t update_cursor(int32_t x, int32_t y)
{
    terminals[cur_terminal].cursor_x = x;
    terminals[cur_terminal].cursor_y = y;
    return 0;
}
/*
interface for this function
input: none
output: 0
effect: move the cursor to the right
*/
int32_t move_cursor_right()
{
    //terminal.cursor_x++;
    if(terminals[cur_terminal].cursor_x>=NUM_COLS) //if the cursor is at the end of the line, then we need to move to the next line
    {
        terminals[cur_terminal].cursor_x=0;
        terminals[cur_terminal].cursor_y++;
    }
    if(terminals[cur_terminal].cursor_y>=NUM_ROWS)     //if the cursor is at the end of the screen, then we need to scroll up
    {
        terminals[cur_terminal].cursor_y=NUM_ROWS-1;
        //scroll_up();
    }
    update_cursor(terminals[cur_terminal].cursor_x,terminals[cur_terminal].cursor_y);
    print_cursor();
    return 0;
}
/*
interface for this function
input: none
output: 0
effect: move the cursor to the left
*/
int32_t move_cursor_left()
{
    terminals[cur_terminal].cursor_x--;
    if(terminals[cur_terminal].cursor_x<0)
    {
        terminals[cur_terminal].cursor_x=NUM_COLS-1;   //if the cursor is at the beginning of the line, then we need to move to the previous line
        terminals[cur_terminal].cursor_y--;
    }
    if(terminals[cur_terminal].cursor_y<0)
    {
        terminals[cur_terminal].cursor_y=0;    //if the cursor is at the beginning of the screen, then we need to move to the previous screen
    }
    update_cursor(terminals[cur_terminal].cursor_x,terminals[cur_terminal].cursor_y); //update the cursor
    return 0;
}

// int32_t print_cursor()  {
//     uint16_t* video_mem = (uint16_t*)VIDEO;
//     video_mem[terminal.cursor_y*NUM_COLS+terminal.cursor_x] = (video_mem[terminal.cursor_y*NUM_COLS+terminal.cursor_x] & 0xFF00) | '_';
//     return 0;

// }

/*
interface for this function
input: cursor_start, cursor_end
output: none
effect: enable the cursor
*/
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end){ // 12 and 14. called in kernel.c
	outb(0x0A, 0x3D4);
	outb((inb(0x3D5) & 0xC0) | cursor_start, 0x3D5);
 
	outb(0x0B, 0x3D4);
	outb((inb(0x3D5) & 0xE0) | cursor_end, 0x3D5);
}
/*
interface for this function
input: none
output: none
effect: disable the cursor
*/
void disable_cursor(){  //called in kernel.c
    outb(0x0A, 0x3D4);
	outb(0x20, 0x3D5);
}
/*
interface for this function
input: none
output: none
effect: print the cursor
*/
void print_cursor(){    //called in keyboard.c, prints the cursor

    uint16_t pos = terminals[cur_terminal].cursor_y * NUM_COLS + terminals[cur_terminal].cursor_x;
 
	outb(0x0F, 0x3D4);
	outb((uint8_t) (pos & 0xFF), 0x3D5);
	outb(0x0E, 0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF), 0x3D5);
}
/*
interface for this function
input: none
output: 0
effect: scroll up the screen
*/
int32_t scroll_up() {   //called in keyboard.c, scrolls up the screen
    int i;
    uint16_t* video_mem = (uint16_t*)VIDEO;
    for(i=0;i<NUM_ROWS*NUM_COLS;i++)
    {
        if(i<NUM_COLS*(NUM_ROWS-1))
        {
            video_mem[i] = video_mem[i+NUM_COLS];   //move the screen up
        }
    }
    clear_last_line(cur_terminal);  //clear the last line
    terminals[cur_terminal].cursor_x = 0;
    terminals[cur_terminal].cursor_y = NUM_ROWS-1;
    update_cursor(terminals[cur_terminal].cursor_x,terminals[cur_terminal].cursor_y);
    print_cursor();
    return 0;
}

void switch_terminal(uint8_t terminal_num)
{
    if(terminal_num==cur_terminal)
    {
        return;
    }
    if(terminal_num>2)
    {
        return;
    }
    vidmem_remap(cur_terminal);
    if(cur_terminal==0)
    {
        memcpy(terminal1_vidaddr, (uint8_t*)VIDEO, 4096);
    }
    else if(cur_terminal==1)
    {
        memcpy(terminal2_vidaddr, (uint8_t*)VIDEO, 4096);
    }
    else if(cur_terminal==2)
    {
        memcpy(terminal3_vidaddr, (uint8_t*)VIDEO, 4096);
    }
    if(terminal_num==0)
    {
        memcpy((uint8_t*)VIDEO, terminal1_vidaddr, 4096);
    }
    else if(terminal_num==1)
    {
        memcpy((uint8_t*)VIDEO, terminal2_vidaddr, 4096);
    }
    else if(terminal_num==2)
    {
        memcpy((uint8_t*)VIDEO, terminal3_vidaddr, 4096);
    }
    //------------------------------------------------unfinished---------------------------------------------------------
    cur_terminal=terminal_num;
    vidmem_remap(curr_schedule);
    update_cursor(terminals[cur_terminal].cursor_x,terminals[cur_terminal].cursor_y);
    print_cursor();
}

