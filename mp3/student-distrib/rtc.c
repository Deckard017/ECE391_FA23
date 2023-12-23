#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"


uint32_t rtc_freq;
volatile uint32_t rtc_interrupt_occured;      //synchronization flag
volatile uint32_t rtc_counter;

uint32_t interval;
/*
    * init_rtc
    *   DESCRIPTION: initialize the rtc
    *   INPUTS: none
    *   OUTPUTS: none
    *   RETURN VALUE: none
    *   SIDE EFFECTS: initialize the rtc
*/
void init_rtc(void)
{
    
    char previous;      //store the previous value of register B
    outb(REG_B,REG_PORT);       // Select register B in the RTC by writing REG_B to the register select port (REG_PORT).
    previous=inb(DATA_PORT);    // Read the current value of register B from the RTC's data port (DATA_PORT) and store it in 'previous'.
    outb(REG_B,REG_PORT);       // To modify the RTC's configuration, select register B again by writing REG_B to the register select port.
    outb(previous|0x40,DATA_PORT);  // Enable the RTC interrupt by setting the 6th bit (0x40) in the 'previous' value and writing it back to the RTC's data port.
    
    outb(REG_A, REG_PORT);
    previous=inb(DATA_PORT);
    outb(REG_A,REG_PORT);
    outb((previous & 0xF0)|6,DATA_PORT);

    //CHECKPOINT 2  
    rtc_freq=2;                //initialize the frequency
    rtc_interrupt_occured=0;    //initialize the flag
    rtc_counter=0;            //initialize the counter
    enable_irq(RTC_IRQ);        //enable the irq
}
/*
    * handle_rtc
    *   DESCRIPTION: handle the rtc
    *   INPUTS: none
    *   OUTPUTS: none
    *   RETURN VALUE: none
    *   SIDE EFFECTS: handle the rtc
*/
void handle_rtc(void)
{
    //printf("rtc\n");
    cli();      //mask interrupt when in this interrupt handler
    char temp;
    rtc_counter++;    //increment the counter
    //the frequency of rtc is virtualized, its actual frequency is 1024Hz
    interval = (HIGHEST_FREQ / rtc_freq);
    if (rtc_counter % interval == 0) {
        rtc_interrupt_occured=1;    //set the flag at the frequency of rtc_freq(virtualized)
    }
    outb(REG_C&0xF,REG_PORT);
    temp=inb(DATA_PORT);    //read register C to reset the interrupt
    send_eoi(RTC_IRQ);
    sti();
}
/*
    * open_rtc
    *   DESCRIPTION: open the rtc
    *   INPUTS: filename
    *   OUTPUTS: none
    *   RETURN VALUE: 0
    *   SIDE EFFECTS: open the rtc
*/
int32_t open_rtc(const uint8_t* filename)
{
    rtc_freq = 2;   //initialize the frequency
    rtc_interrupt_occured=0;    //initialize the flag
    return 0;

}
/*
    * close_rtc
    *   DESCRIPTION: close the rtc
    *   INPUTS: fd
    *   OUTPUTS: none
    *   RETURN VALUE: 0
    *   SIDE EFFECTS: close the rtc
*/
int32_t close_rtc(int32_t fd)
{
    return 0;
}

/*
    * write_rtc
    *   DESCRIPTION: write to the rtc
    *   INPUTS: fd, buf, nbytes
    *   OUTPUTS: none
    *   RETURN VALUE: 0
    *   SIDE EFFECTS: write to the rtc
*/
 int32_t write_rtc(int32_t fd, const void* buf, int32_t nbytes) {
    uint32_t temp = *(uint32_t*) buf;
    if (temp < 2 || temp > HIGHEST_FREQ || !isPowerOf2(temp)) { //check if the frequency is valid
        return -1;
    }
    rtc_freq = temp;    //set the frequency
    return 0;
 }
/*
    * read_rtc
    *   DESCRIPTION: read from the rtc
    *   INPUTS: fd, buf, nbytes
    *   OUTPUTS: none
    *   RETURN VALUE: 0
    *   SIDE EFFECTS: read from the rtc
*/
 int32_t read_rtc(int32_t fd, void* buf, int32_t nbytes) {
    while (rtc_interrupt_occured == 0); //wait until the interrupt occurs
    rtc_interrupt_occured=0;    //reset the flag
    return 0;
 }
/*
    * isPowerOf2
    *   DESCRIPTION: check if the number is power of 2
    *   INPUTS: number
    *   OUTPUTS: none
    *   RETURN VALUE: 0 or 1
    *   SIDE EFFECTS: check if the number is power of 2
*/
uint8_t isPowerOf2(uint32_t number) {
    if (number == 0) {
        return 0; // 0 is not a power of 2
    }
    return (number & (number - 1)) == 0;
}









// int32_t get_the_rate(int32_t frequency)
// {
//      switch(frequency)
//      {
//         case 2:     return 0x0F;
//         case 4:     return 0x0E;
//         case 8:     return 0x0D;
//         case 16:    return 0x0C;
//         case 32:    return 0x0B;
//         case 64:    return 0x0A;
//         case 128:   return 0x09;
//         case 256:   return 0x08;
//         case 512:   return 0x07;
//         case 1024:  return 0x06;
//         // case 2048:  return 0x05;
//         // case 4096:  return 0x04;
//         // case 8192:  return 0x03;
//         default:    return -1;
//      }

// }
