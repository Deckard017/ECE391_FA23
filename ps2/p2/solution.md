### P2 Solution

# 1.

### MTCP_BIOC_ON

**When it should be sent:** When the user wants to create an interrupt whenever he pushes or releases a button, when he wants to change the state of the program.

**What effects it has on the device:** Enable Button interrupt-on-change.

**Messages returned:** MTCP_ACK

### MTCP_LED_SET

**When it should be sent:** When the user wants to change the LED display.

**What effects it has on the device:** Set the user-set LED display values, which will be displayed on the LED displays when the LED display is in USR mode. The argument specifies which LEDs to set as well as their display values.

**Messages returned:** MTCP_ACK

# 2.

### MTCP_ACK

**When the device sends the message:** When commands MTCP_BIOC_ON, MTCP_BIOC_OFF, MTCP_DBG_OFF or MTCP_LED_SET is completed.

**Information conveyed:** A command by the user is successfully executed.

### MTCP_BIOC_EVENT

**When the device sends the message:** When the Button Interrupt-on-change mode is enabled and a button is either pressed or released. It's the Byte 0 of MTCP_BIOC_EVT, generated in such an occasion.

**Information conveyed:** A button has been pressed or released and it generates an interrupt.

### MTCP_RESET

**When the device sends the message:** When the devide re-initializes itself after a power-up, a RESET button press, or an MTCP_RESET_DEV command.

**Information conveyed:** The device has finished initializing itself.

# 3.

Because the function's caller, *tuxctl_ldisc_data_callback*, is called from an interrupt context, and it cannot acquire any semaphores or otherwise sleep or take up too much time, and since the ioctls would have to wait a long time(9ms) for responses to the commands they send to the controller, which is too long for a system call to take, instead they should return immediately with success if their parameters are valid.