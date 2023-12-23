#ifndef RTC_H
#define RTC_H


#include "types.h"
#include "lib.h"
#include "i8259.h"

#define RTC_IRQ 8
#define REG_PORT    0x70
#define DATA_PORT    0x71
#define REG_A 0x8A
#define REG_B 0x8B
#define REG_C 0x8C
#define first_half_mask    0xF0
#define HIGHEST_FREQ 1024

/*The function to initialize rtc*/
extern void init_rtc(void);
/*The function to handle rtc*/
extern void handle_rtc(void);
/*The function to rtc open*/
extern int32_t open_rtc(const uint8_t* filename);
/*The function to rtc close*/
extern int32_t close_rtc(int32_t fd);
/*The function to read from rtc*/
extern int32_t read_rtc(int32_t fd, void* buf, int32_t nbytes);
/*The function to write from rtc*/
extern int32_t write_rtc(int32_t fd, const void* buf, int32_t nbytes);
//helper function
extern uint8_t isPowerOf2(uint32_t number);


#endif
