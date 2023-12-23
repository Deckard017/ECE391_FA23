#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"
#include "terminal.h"

#define LCTRL_PRESSED 0x1D      //left control pressed
#define LCTRL_RELEASED 0x9D     //left control released
#define LSHIFT_PRESSED 0x2A     //left shift pressed
#define LSHIFT_RELEASED 0xAA        //left shift released
#define RSHIFT_PRESSED 0x36     //right shift pressed
#define RSHIFT_RELEASED 0xB6    //right shift released
#define CAPS_PRESSED 0x3A       //caps lock pressed
#define NO_S_C 0        //no shift and no caps
#define SHIFT 1     //shift
#define CAPS 2      //caps
#define SHIFT_CAPS 3        //shift and caps
#define LALT_PRESSED 0x38       //left alt pressed
#define LALT_RELEASED 0xB8      //left alt released
#define F1_PRESSED 0x3B     //F1 pressed
#define F2_PRESSED 0x3C     //F2 pressed
#define F3_PRESSED 0x3D     //F3 pressed


uint8_t caps_lock = 0;
uint8_t shift = 0;
uint8_t ctrl = 0;
uint8_t alt = 0;
char scan_code_tables_transposed[0x3B][4] = {       //four different keyboard statuses
    {'\0', '\0', '\0', '\0'},
    {'\0', '\0', '\0', '\0'},
    {'1', '!', '1', '!'},
    {'2', '@', '2', '@'},
    {'3', '#', '3', '#'},
    {'4', '$', '4', '$'},
    {'5', '%', '5', '%'},
    {'6', '^', '6', '^'},
    {'7', '&', '7', '&'},
    {'8', '*', '8', '*'},
    {'9', '(', '9', '('},
    {'0', ')', '0', ')'},
    {'-', '_', '-', '_'},
    {'=', '+', '=', '+'},
    {'\b', '\b', '\b', '\b'},
    {'\t', '\t', '\t', '\t'},
    {'q', 'Q', 'Q', 'q'},
    {'w', 'W', 'W', 'w'},
    {'e', 'E', 'E', 'e'},
    {'r', 'R', 'R', 'r'},
    {'t', 'T', 'T', 't'},
    {'y', 'Y', 'Y', 'y'},
    {'u', 'U', 'U', 'u'},
    {'i', 'I', 'I', 'i'},
    {'o', 'O', 'O', 'o'},
    {'p', 'P', 'P', 'p'},
    {'[', '{', '[', '{'},
    {']', '}', ']', '}'},
    {'\n', '\n', '\n', '\n'},
    {'\0', '\0', '\0', '\0'},
    {'a', 'A', 'A', 'a'},
    {'s', 'S', 'S', 's'},
    {'d', 'D', 'D', 'd'},
    {'f', 'F', 'F', 'f'},
    {'g', 'G', 'G', 'g'},
    {'h', 'H', 'H', 'h'},
    {'j', 'J', 'J', 'j'},
    {'k', 'K', 'K', 'k'},
    {'l', 'L', 'L', 'l'},
    {';', ':', ';', ':'},
    {'\'', '\"', '\'', '\"'},
    {'`', '~', '`', '~'},
    {'\0', '\0', '\0', '\0'},
    {'\\', '|', '\\', '|'},
    {'z', 'Z', 'Z', 'z'},
    {'x', 'X', 'X', 'x'},
    {'c', 'C', 'C', 'c'},
    {'v', 'V', 'V', 'v'},
    {'b', 'B', 'B', 'b'},
    {'n', 'N', 'N', 'n'},
    {'m', 'M', 'M', 'm'},
    {',', '<', ',', '<'},
    {'.', '>', '.', '>'},
    {'/', '?', '/', '?'},
    {'\0', '\0', '\0', '\0'},
    {'\0', '\0', '\0', '\0'},
    {'\0', '\0', '\0', '\0'},
    {' ', ' ', ' ', ' '},
    {'\0', '\0', '\0', '\0'}
};

//uint8_t scan_code_to_char[26];


/*The function to initialize the keyboard device*/
void init_keyboard(void)
{
    enable_irq(KEYBOARD_IRQ);
}
/*The function to handle keyboard*/
void handle_keyboard(void){
     cli();
    uint8_t scan_code;
    uint8_t key;
    terminal_t* curr_terminal = get_terminal();
    if(curr_terminal==NULL)
    {
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }
    scan_code = inb(KEYBOARD_PORT);
    //map the scan code to ascii
    //uint8_t ascii = scan_code_to_char[scan_code];
    switch(scan_code)
    {
        //handle special keys
        case LCTRL_PRESSED:
            ctrl = 1;
            break;
        case LCTRL_RELEASED:
            ctrl = 0;
            break;
        case LSHIFT_PRESSED:
            shift = 1;
            break;
        case RSHIFT_PRESSED:
            shift = 1;
            break;
        case LSHIFT_RELEASED:
            shift = 0;
            break;
        case RSHIFT_RELEASED:
            shift = 0;
            break;
        case CAPS_PRESSED:
            caps_lock=!caps_lock;
            break;
        case LALT_PRESSED:
            alt = 1;
            break;
        case LALT_RELEASED:
            alt = 0;
            break;
        case F1_PRESSED:
            if(alt==1)
            {
                switch_terminal(0);
            }
            break;
        case F2_PRESSED:
            if(alt==1)
            {
                switch_terminal(1);
            }
            break;
        case F3_PRESSED:
            if(alt==1)
            {
                switch_terminal(2);
            }
            break;
        default:
            // if(curr_terminal->count>=128)
            // {
            //     break;
            // }
            if(scan_code>=59)   //ignore the keys that are not in the table
            {
                break;
            }
            if(shift==1)        //handle shift and caps lock
            {
                key=scan_code_tables_transposed[scan_code][SHIFT];
            }
            if(caps_lock==1)    //handle caps lock
            {
                key=scan_code_tables_transposed[scan_code][CAPS];
            }
            if(caps_lock==1 && shift==1)    //handle shift and caps lock
            {
                key=scan_code_tables_transposed[scan_code][SHIFT_CAPS];
            }
            if(caps_lock==0 && shift==0)    //handle no shift and no caps
            {
                key=scan_code_tables_transposed[scan_code][NO_S_C];
            }
            if(ctrl==1 && (key=='l'||key=='L'))   //handle ctrl+l
            {
                clear();
                int j;
                for(j=0;j<curr_terminal->count;j++)
                {
                    curr_terminal->keyboard_buffer[j]='\0'; //clear the buffer
                }
                curr_terminal->count=0; //reset the count
            }
            if(key=='\n')   //handle enter
            {
                putc_user(key);
                
                curr_terminal->keyboard_buffer[curr_terminal->count++]=key; //put the enter into the buffer
                curr_terminal->flag_for_read = 1;   //set the flag for read
                curr_terminal->count=0; //reset the count
                curr_terminal->cursor_x = get_screen_x(cur_terminal);   //reset the cursor
                curr_terminal->cursor_y = get_screen_y(cur_terminal);   
                if(curr_terminal->cursor_y>24)  //scroll up
                {
                    //scroll_up();
                }
                print_cursor();
            }
            if(key=='\b')   //handle backspace
            {
                if(curr_terminal->count>0)  //if the count is greater than 0, then we can handle backspace
                {
                    //putc_user(8);
                    //move_cursor_left();
                    //print_cursor();
                    /*handle backspace*/
                    curr_terminal->count--;
                    handle_backspace(cur_terminal);
                    //curr_terminal->keyboard_buffer[curr_terminal->count]=' ';
                    
                }
                
            }
            if(key=='\t')   //handle tab
            {
                int i;
                for(i=0;i<4;i++)
                {
                    if(curr_terminal->count<127)
                    {
                        //put_in_newline();

                        putc_user(' ');
                        move_cursor_right();
                        print_cursor();
                        curr_terminal->keyboard_buffer[curr_terminal->count++]=' ';
                       
                    }
                }
            }
            if(key!='\n'&&key!='\b'&&key!='\t' && !(ctrl && key =='l')) //handle other keys
            {
                if (curr_terminal->count<127) {
                    putc_user(key);
                    move_cursor_right();    //move the cursor right
                    print_cursor(); //print the cursor
                    
                    curr_terminal->keyboard_buffer[curr_terminal->count++]=key;
                }
            }
 
    }
    send_eoi(KEYBOARD_IRQ);
    sti();
}
