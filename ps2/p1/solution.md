### P1 Solution

a. To add a non-scrolling status bar using VGA registers:

The split screen start scan line is a 10-bit value, with bits 7-0 stored in the Line Compare register (CRTC register 18H), bit 8 stored in bit 4 of the Overflow register (CRTC register 7) and bit 9 stored in bit 6 of the Maximum Scan Line register (CRTC register 9). 

Set The Start Address High Register to be the number of lines times bytes per row. And set Preset Row Scan field 1 to achieve scrolling for the main window.

A split screen start scan line value is programmed into three VGA registers. At the beginning of each frame, the video circuitry begins to scan display memory for video data starting at the address specified by the start address registers. When the video circuitry encounters the specified split screen start scan line while scanning video data onto the screen, it completes that scan line normally, then resets the internal pointer which addresses the next byte of display memory to be read for video data to zero. Display memory from address zero onward is then scanned for video data in the usual way, progressing toward the high end of memory. At the end of the frame, the pointer to the next byte of display memory to scan is reloaded from the start address registers, and the whole process starts over.

Constraints:
- The bottom window's starting display memory address is fixed at 0.
- Preset Valid values of Preset Row Scan field range from 0 to the value of the Maximum Scan Line field.
- Either both windows are panned by the same amount, or only the top pans.

b. To change the VGA's color palette:

- Disable interrupts and acquire a spinlock to prevent other threads from accessing the VGA registers.
- Write the index of the color you want to change to the DAC Address Write Mode Register (port 0x3C8).
- Write the red, green, and blue values of the color (each in 6 bits) to the DAC Data Register (port 0x3C9) in that order. The internal address will automatically increment after each write.
- Release the spinlock and enable interrupts.