#ifndef SYSCALL_H
#define SYSCALL_H
#include "types.h"
#include "lib.h"
#include "x86_desc.h"
#include "page.h"
#include "keyboard.h"
#include "rtc.h"
#include "filesystem.h"
#include "terminal.h"

#define ELF 0x7f
#define E 0x45
#define L 0x4c
#define F 0x46


typedef struct file_operation_table_t
{
    /* data */
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
} file_operation_table_t;

typedef struct file_descriptor_t
{
    /* data */
    file_operation_table_t* file_operation_table_ptr;
    int32_t inode;
    int32_t file_position;
    int32_t flags;
} file_descriptor_t;

typedef struct pcb_t
{
    /* data */
    uint32_t pid;
    uint32_t parent_pid;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esp0;
    uint32_t vidmap;
    uint32_t ss0;
    int8_t args[128];
    file_descriptor_t fd_array[8];
    int32_t ebp_for_schedule;
} pcb_t;



pcb_t* get_pcb();

int32_t execute(const uint8_t* command);
int32_t halt(uint8_t status);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t getargs(uint8_t* buf, int32_t nbytes);
int32_t vidmap(uint8_t** screen_start);
int32_t set_handler(int32_t signum, void* handler_address);
int32_t sigreturn(void);

pcb_t* init_pcb(int8_t* arg, int32_t next_process);
void helper_function_of_iret();
void helper_function_of_check_exit();
void helper_function_of_reset_schedule(int32_t current_pid);
void helper_function_of_close_file();
int32_t helper_function_of_load_program(const uint8_t* filename);
int32_t helper_function_of_executable_check(const uint8_t* filename);
int32_t helper_function_of_args(const uint8_t* command, uint8_t* name_of_file, int8_t* arg);
extern pcb_t* helper_function_of_get_curr_pcb();



file_operation_table_t TERMINAL_driver;
file_operation_table_t RTC_driver;
file_operation_table_t FILE_driver;
file_operation_table_t DIR_driver;


int32_t init_file_operation_table();

#endif
