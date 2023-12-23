/* tuxctl-ioctl.c
 *
 * Driver (skeleton) for the mp2 tuxcontrollers for ECE391 at UIUC.
 *
 * Mark Murphy 2006
 * Andrew Ofisher 2007
 * Steve Lumetta 12-13 Sep 2009
 * Puskar Naha 2013
 */

#include <asm/current.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/miscdevice.h>
#include <linux/kdev_t.h>
#include <linux/tty.h>
#include <linux/spinlock.h>

#include "tuxctl-ld.h"
#include "tuxctl-ioctl.h"
#include "mtcp.h"

#define debug(str, ...) \
	printk(KERN_DEBUG "%s: " str, __FUNCTION__, ## __VA_ARGS__)


static bool flag_for_busy=1;							// This is for checking whether the tux is busy.
static spinlock_t lock;									// This is an spinlock that avoid other changing the shared data.
static int irqflag;										// This is the flag used for irqsaved and irqrestore.

static unsigned char mask_LED;							// This is the mask for deciding which LED will show on the screen.
static unsigned char buttons_bits;						// This is used for storing the bits of buttons.
static unsigned char LEDs[4];							// This is for the 4 LEDs.	
const unsigned char seg7[16]={0xE7, 0x06, 0xCB, 0x8F, 0x2E, 0xAD, 0xED, 0x86, 0xEF, 0xAE, 0xEE, 0x6D, 0xE1, 0x4F, 0xE9, 0xE8};

// This is a const unsigned char array that show the mapping rule of decimal to hex value shown on the screen.


/**
 * packet_bioc_event - Handle button events from the Tux Controller packet.
 *	Inputs:
 * 		b: Button data from the first packet byte.
 * 		c: Button data from the second packet byte.
 *
 * 		This function processes button events received from the Tux Controller's
 * 		Basic Input/Output Control (BIOC) packet. It maps the button data from
 * 		the packet to the final button bits representation, and updates the
 * 		'buttons_bits' variable accordingly.
 *
 * 		Outputs: none
 * 		Side effects: none
 */

void packet_bioc_event(unsigned b, unsigned c)			// This is the helper function of handling the button.
{
	int r,l,d,u;										// Eight temporary variables for the button.
	int C,B,A,S;
	if(b!=0x80 || c!=0x80)
	{
		unsigned b_mask,c_mask;						// This part is to assign the eight buttons to the final buttons bits.
		b_mask=b & 0x0F;
		c_mask=c & 0x0F;
		C=(b_mask & 0x08) >> 3;						// The mapping rule is came from pdf.
		B=(b_mask & 0x04) >> 2;
		A=(b_mask & 0x02) >> 1;
		S=(b_mask & 0x01);
		r=(c_mask & 0x08) >> 3;
		d=(c_mask & 0x04) >> 2;
		l=(c_mask & 0x02) >> 1;
		u=(c_mask & 0x01);
		
		buttons_bits=(r<<7)+(l<<6)+(d<<5)+(u<<4)+(C<<3)+(B<<2)+(A<<1)+S;
		//get_buttons_bits(buttons1);
		spin_unlock_irqrestore(&lock, irqflag);
	}
}

/**
 * tuxctl_handle_reset - Handle a reset request for the Tux Controller.
 * Inputs: 
 * 		tty: Pointer to the tty_struct representing the TTY device.
 *
 * 		This function handles a reset request for the Tux Controller by sending a
 * 		series of control commands to reset the controller and restore the LED
 * 		display to the last known state. It also updates the internal flags.
 *		
 * 		Outpus: none
 * 		Return value: none
 *		Side effect: none
 */

void tuxctl_handle_reset (struct tty_struct* tty) {

	unsigned char buff[8]={MTCP_BIOC_ON,MTCP_LED_USR,MTCP_LED_SET,0x0F,0x00,0x00,0x00,0x00};		// Opcodes array.
	buff[4]=LEDs[0];
	buff[5]=LEDs[1];							// Set value of LEDs to opcodes array.
	buff[6]=LEDs[2];
	buff[7]=LEDs[3];
	if(flag_for_busy==0) 					// Check for busy and return.
	{
		return;
	}
	tuxctl_ldisc_put(tty,buff,8);			
	flag_for_busy=0;				// Set this flag to 0.
}

/* tuxctl_handle_packet()
 * IMPORTANT : Read the header for tuxctl_ldisc_data_callback() in 
 * tuxctl-ld.c. It calls this function, so all warnings there apply 
 * here as well.
 */
void tuxctl_handle_packet (struct tty_struct* tty, unsigned char* cmd)
{
    unsigned a, b, c;
    a = cmd[0]; /* Avoid printk() sign extending the 8-bit */
    b = cmd[1]; /* values when printing them. */
    c = cmd[2];
	switch (a) {											
	case MTCP_ACK:						// Set the flag to 1. 
		flag_for_busy=1;
		break;
	case MTCP_BIOC_EVENT:				// Handle the buttons.
		packet_bioc_event(b, c);
		break;
	case MTCP_RESET:					// Reset Situation.
		tuxctl_handle_reset(tty);	
		break;
	default:				
		break;
	}
}

/******** IMPORTANT NOTE: READ THIS BEFORE IMPLEMENTING THE IOCTLS ************
 *                                                                            *
 * The ioctls should not spend any time waiting for responses to the commands *
 * they send to the controller. The data is sent over the serial line at      *
 * 9600 BAUD. At this rate, a byte takes approximately 1 millisecond to       *
 * transmit; this means that there will be about 9 milliseconds between       *
 * the time you request that the low-level serial driver send the             *
 * 6-byte SET_LEDS cmd and the time the 3-byte ACK cmd finishes               *
 * arriving. This is far too long a time for a system call to take. The       *
 * ioctls should return immediately with success if their parameters are      *
 * valid.                                                                     *
 *                                                                            *
 ******************************************************************************/

/**
 * tuxctl_ioctl_init - Initialize the Tux Controller driver.
 * 		Input: tty: Pointer to the tty_struct representing the TTY device.
 *
 * 		This function initializes the Tux Controller driver by sending a series
 * 		of control commands to the Tux Controller device. It sets the device in
 * 		BIOC (Basic Input/Output Control) mode, configures the LED display to user
 * 		mode, and clears any pending LED settings. By the way, it initializes
 * 		internal flags and data structures.
 *		Outputs: none
 * 		Return Value: 0 on success, or an error code on failure.
 *		Side effects: none
 */

int tuxctl_ioctl_init (struct tty_struct* tty) {

	unsigned char rest_buf[4]={MTCP_BIOC_ON, MTCP_LED_USR, MTCP_LED_SET, 0x00};   
	if(!flag_for_busy)										// Check if the driver is busy with another operation.
	{
		return 0;    										// Driver is busy, return success.
	}
	tuxctl_ldisc_put(tty, rest_buf, 4);  					// Send control commands to initialize the Tux Controller.
	flag_for_busy=0;        								// Reset internal flags and data.
	buttons_bits=0xFF;
	lock=SPIN_LOCK_UNLOCKED;
	return 0;
}
/**
 * tuxctl_ioctl_buttons - Get the state of Tux Controller buttons.
 * 		Inputs: tmp: Pointer to store the button state.
 *
 * 		This function retrieves the current state of the Tux Controller buttons
 * 		and stores it in the variable pointed to by 'tmp'. It protects the
 * 		critical section using spin locks to ensure atomic access to the
 * 		'buttons_bits' variable.
 *
 * 		Outputs: none
 *
 * 		Return Value: 0 on success, or -EINVAL if 'tmp' is NULL.
 *
 *		Side effects: none
 */

 int tuxctl_ioctl_buttons(unsigned long* tmp) 			
 {
	if(!tmp) 												// If tmp is NULL
	{
		return -EINVAL;			
	}					
	spin_lock_irqsave(&lock, irqflag);						// Store the flag.
	*tmp=(unsigned long) buttons_bits;						// Copy the button state to 'tmp'.
	spin_unlock_irqrestore(&lock, irqflag);					// Restore.
	return 0;
}

/**
 * tuxctl_ioctl_set_led - Set the LED display on the Tux Controller.
 * Inputs:
 * 		tty: Pointer to the tty_struct representing the TTY device.
 * 		arg: Argument specifying the LED configuration.
 *
 * 		This function sets the LED display on the Tux Controller based on the
 * 		provided argument. It takes the LED configuration argument, extracts
 * 		information about the segments to display, decimal points, and LED mask,
 * 		and sends the appropriate command to the Tux Controller.
 *		
 *		Outpus: none
 * 		Return values: 0 on success, or an error code on failure.
 *		Side effects: none.
 */

int tuxctl_ioctl_set_led(struct tty_struct* tty, unsigned long arg) {
    unsigned char cmd[6]={MTCP_LED_SET,0x0F,0,0,0,0};
	int num0,num1,num2,num3;						// temprorary variables.
	if(flag_for_busy==0) 							// Check if the driver is not busy, if so return 0.
	{
		return 0;
	}
	mask_LED=(arg >> 16) & 0x000F;					// Get the mask bits of 4 LEDs.
	num0=(arg >> 0) & 0x0F;
	num1=(arg >> 4) & 0x0F;
	num2=(arg >> 8) & 0x0F;
	num3=(arg >> 12) & 0x0F;
													// Set segments and decimal points for each digit and set decimal.
	cmd[2]=seg7[num0];
	if((arg >> 24) & 1) 
	{
		cmd[2] |= 0x10;
	}
	LEDs[0]=cmd[2];
	if(!((mask_LED >> 0) & 1)) 
	{
		cmd[2]=0;
	}
													// Set segments and decimal points for each digit and set decimal.
	cmd[3]=seg7[num1];
	if((arg >> 25) & 1) 
	{
		cmd[3] |= 0x10;
	}
	LEDs[1]=cmd[3];
	if(!((mask_LED >> 1) & 1)) 
	{
		cmd[3]=0;
	}
													// Set segments and decimal points for each digit and set decimal.
	cmd[4]=seg7[num2];
	if((arg >> 26) & 1)
	{
		cmd[4] |= 0x10;
	}
	LEDs[2]=cmd[4];
	if(!((mask_LED >> 2) & 1)) 
	{
		cmd[4]=0;
	}
													// Set segments and decimal points for each digit and set decimal.
	cmd[5]=seg7[num3];
	if((arg >> 27) & 1) 
	{
		cmd[5] |= 0x10;
	}
	LEDs[3]=cmd[5];
	if(!((mask_LED >> 3) & 1)) 
	{
		cmd[5]=0;
	}
	tuxctl_ldisc_put(tty,cmd,6);
	flag_for_busy=0;
	return 0;
}

/**
 * tuxctl_ioctl - Tux Controller ioctl handler.
 *		
 *		Inputs:
 * 		tty: Pointer to the tty_struct representing the TTY device.
 * 		file: Pointer to the file structure.
 *		cmd: IOCTL command code.
 * 		arg: Argument associated with the command.
 *
 * 		This function serves as the ioctl handler for the Tux Controller driver.
 * 		It handles various ioctl commands related to Tux Controller initialization,
 * 		button state retrieval, and LED configuration.
 *			
 *		Outputs: none
 * 		Return value: 0 on success, or an error code on failure.
 *		Side effect: none
 */

int tuxctl_ioctl(struct tty_struct* tty, struct file* file, 
	      unsigned cmd, unsigned long arg) 
{
	switch (cmd) 							// Three cases
	{
		case TUX_INIT:			
			return tuxctl_ioctl_init(tty);
		case TUX_BUTTONS:		
			return tuxctl_ioctl_buttons((unsigned long*) arg);
		
		case TUX_SET_LED:		
			return tuxctl_ioctl_set_led(tty, arg);
		default:				
			return -EINVAL;
    }
}
