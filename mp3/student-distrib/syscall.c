#include "syscall.h"
#include "lib.h"
#include "schedule.h"
uint8_t pid_mask[6]={0,0,0,0,0,0};

int32_t process_address;

/*
Initialize the file operation table
*/
int32_t init_file_operation_table()     //initialize the file operation table
{
    //TERMINAL_driver
    TERMINAL_driver.open=&terminal_open;
    TERMINAL_driver.close=&terminal_close;
    TERMINAL_driver.read=&terminal_read;
    TERMINAL_driver.write=&terminal_write;
    //RTC_driver
    RTC_driver.open=&open_rtc;
    RTC_driver.close=&close_rtc;
    RTC_driver.read=&read_rtc;
    RTC_driver.write=&write_rtc;
    //FILE_driver
    FILE_driver.open=&fopen;
    FILE_driver.close=&fclose;
    FILE_driver.read=&fread;   
    FILE_driver.write=&fwrite;
    //DIR_driver
    DIR_driver.open=&dopen;
    DIR_driver.close=&dclose;
    DIR_driver.read=&dread;
    DIR_driver.write=&dwrite;
    return 0;
}

/*
 * execute
 *   DESCRIPTION: execute the command
 *   INPUTS: command -- the command to execute   
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: execute the command 
*/
int32_t execute(const uint8_t* command)
{

    cli();                                                                //disable interrupt
    pcb_t* cur_pcb=NULL;
    int32_t next_process=-1;                                            //next process
    uint32_t index_of_one_pid=0;                                      //index of the page directory entry
    int32_t check_flag=0;                                            //check flag
    if(command==NULL)                                            //check if the command is valid
    {
        return -1;
    }
    uint8_t name_of_file[33];                                    //name of the file
    int8_t arg[128];                                        //argument
    check_flag=helper_function_of_args(command,name_of_file,arg);           //parse args
    if(check_flag==-1)  
    {
        // printf("A");
        return -1;
    }
    check_flag=0;
    check_flag=helper_function_of_executable_check(name_of_file);            //check if the file is executable
    if(check_flag==-1)
    {
        //printf("B");
        return -1;
    }
    uint32_t start_pid=0;                                        //start pid
    //uint32_t index_of_process;
    while(pid_mask[start_pid]!=0)                                //find the next available pid
    {
        start_pid++;
        if(start_pid==6)
        {
            start_pid= -1;
            break;
        }
    }
    if(start_pid==-1)                                        //if no more process can be executed
    {
        printf("No more process can be executed\n");
        return 0;
    }
    pid_mask[start_pid]=1;                                  //set the pid mask
    
    next_process=start_pid;                                //set the next process
    index_of_one_pid=0x20;                                //index of the page directory entry
    set_PDE_mb(index_of_one_pid,1,1,1,0,2+start_pid);    //set up the page directory entry
    //flush TLB
    asm volatile(
        "movl %%cr3,%%eax     ;"
        "movl %%eax,%%cr3     ;"
        : : : "eax", "cc" 
    );
    check_flag=0;
    check_flag=helper_function_of_load_program(name_of_file);                //load the program into memory
    if(check_flag==-1)
    {
        return -1;
    }
    //printf("B");
    cur_pcb=init_pcb(arg, next_process);                                                        //initialize the pcb
    if(cur_pcb->parent_pid!=-1)                                            //store the esp and ebp of parent process
    {
        //int32_t pid_to_store=cur_pcb->parent_pid;
        // pcb_t* parent_pcb=(pcb_t*)(0x800000-0x2000*(pid_to_store+1));
        pcb_t* parent_pcb=(pcb_t*)(0x800000-0x2000*(cur_pcb->parent_pid+1));    //get the parent pcb
        uint32_t parent_esp;    //esp of parent process
        uint32_t parent_ebp;    //ebp of parent process
        asm volatile(        //store the esp and ebp of parent process
            "movl %%ebp, %0 ;"
            "movl %%esp, %1 ;"
            : "=r" (parent_ebp) ,"=r" (parent_esp) 
        );
        parent_pcb->esp=parent_esp;   //store the esp of parent process
        parent_pcb->ebp=parent_ebp;   //store the ebp of parent process
    }
    //helper_function_of_iret();
    //context switch
    tss.ss0=KERNEL_DS;
    tss.esp0=cur_pcb->esp0;
    sti();
    asm volatile("pushl %0 \n\t"
                 "pushl %1 \n\t"
                  "pushfl   \n\t"
                  "pushl %2 \n\t"
                  "pushl %3 \n\t"
                  "IRET     \n\t"
                :
                : "r" (USER_DS), 
                  "r" (0x08000000+0x400000-4), 
                  "r" (USER_CS), 
                  "r" (process_address)
                : "memory");
    //printf("B");
    return 0;
}


/*
* helper_function_of_args
*   DESCRIPTION: parse the args
*   INPUTS: command -- the command to parse
*           name_of_file -- the name of the file
*           arg -- the argument
*   OUTPUTS: none
*   RETURN VALUE: 0
*/
int32_t helper_function_of_args(const uint8_t* command, uint8_t* name_of_file, int8_t* arg)
{
    int i;
    uint32_t start_index=0;
    uint32_t length_of_file_name=0;
    uint32_t temp=0;
    uint32_t length_of_arg=0;
    //printf(strlen((int8_t*)command));
    if(command==NULL)   //check if the command is valid
    {
        return -1;
    }
    while(1)
    {
        if(command[start_index]==' ')   //skip the space
        {
            start_index++;
        }
        else
        {
            break;
        }
    }
    for(i=start_index;i<strlen((int8_t*)command);i++)   //get the length of the file name
    {
        if(command[i]==' ')  //if meet the space, break
        {
            break;
        }
        else
        {

        }
        if(length_of_file_name>=31) //if the length of the file name is too long, return -1
        {
            return -1;
        }
        else
        {
            length_of_file_name++;
            //start_index++;
            name_of_file[i-start_index]=command[i]; //store the file name
        }
        temp=i;
    }
    start_index=temp+1;
    name_of_file[length_of_file_name]='\0'; //set the end of the file name to null

    while(1)
    {
        if(command[start_index]==' ')   //skip the space
        {
            start_index++;
        }
        else
        {
            break;
        }
    }
    for(i=start_index;i<strlen((int8_t*)command);i++)   //get the length of the argument
    {
        if(length_of_arg>=127)  //if the length of the argument is too long, return -1
        {
            return -1;
        }
        else
        {
            length_of_arg++;
            //start_index++;
            arg[i-start_index]=command[i];  //store the argument
        }
    }
    arg[length_of_arg]='\0';    //set the end of the argument to null
    return 0;
}

/*
* helper_function_of_executable_check
*   DESCRIPTION: check if the file is executable
*   INPUTS: filename -- the name of the file
*   OUTPUTS: none
*   RETURN VALUE: 0
*/

int32_t helper_function_of_executable_check(const uint8_t* filename)
{
    dentry_t dentry_of_file;
    int32_t check_flag=0;
    uint8_t elf[4];
    check_flag=read_dentry_by_name(filename,&dentry_of_file);   //read the dentry
    if(check_flag==-1)
    {
        return -1;
    }
    if(4!=read_data(dentry_of_file.inode_num,0,elf,4))  //read the first 4 bytes of the file
    {
        return -1;
    }

    //debug
   

    if(elf[0]!=ELF) //check if the file is executable
    {
        return -1;
    }
    if(elf[1]!=E)
    {
        return -1;
    }
    if(elf[2]!=L)
    {
        return -1;
    }
    if(elf[3]!=F)
    {
        return -1;
    }
    read_data(dentry_of_file.inode_num, 24, (uint8_t*)(&process_address), 4);   //read the entry point of the file
    return 0;
}
/*
* helper_function_of_load_program
*   DESCRIPTION: load the program into memory
*   INPUTS: filename -- the name of the file
*   OUTPUTS: none
*   RETURN VALUE: 0
*/
int32_t helper_function_of_load_program(const uint8_t* filename)
{
    dentry_t dentry_of_user_file;
    int32_t check_flag=0;
    uint32_t length_of_file=0;
    uint8_t buf[4096];
    uint32_t temp=0;
    uint32_t tot_count=0;
    check_flag=read_dentry_by_name(filename,&dentry_of_user_file);  //read the dentry
    if(check_flag==-1)
    {
        return -1;
    }
    length_of_file=get_inode_length(dentry_of_user_file.inode_num); //get the length of the file
    if(length_of_file==-1)
    {
        return -1;
    }
    while(tot_count!=length_of_file)
    {
        temp=read_data(dentry_of_user_file.inode_num,tot_count,buf,4096);   //read the data of the file
        memcpy((void*)(0x08048000+tot_count),buf,temp); //copy the data to the memory
        tot_count+=temp;    //increment the total count
    }
    return 0;
}

/*
* init_pcb
*   DESCRIPTION: initialize the pcb
*   INPUTS: arg -- the argument
*   OUTPUTS: none
*   RETURN VALUE: 0
*/
pcb_t* init_pcb(int8_t* arg, int32_t next_process) 
{
    uint32_t check=0;
    // uint32_t ptr_of_next_pcb=0x800000-0x2000*(next_process+1);  //pointer of the next pcb
    // pcb_t* pcb_ptr=(pcb_t*)ptr_of_next_pcb; //get the pointer of the next pcb
    pcb_t* pcb_ptr=(pcb_t*)(0x800000-0x2000*(next_process+1)); //get the pointer of the next pcb
    
    pcb_ptr->pid=next_process;  //set the pid

    if(next_process==0 || next_process ==1 || next_process ==2)
    {
        pcb_ptr->parent_pid=255;
    }
    else
    {
        register int32_t esp asm("esp");
        pcb_ptr->parent_pid=((0x800000-esp)/0x2000);
    }
    //pcb_ptr->parent_pid=current_process;    //set the parent pid
    pcb_ptr->vidmap=0;
    //uint32_t address_of_next_pcb=(uint32_t) pcb_ptr;
    //pcb_ptr->esp0=address_of_next_pcb+0x2000-4;
    pcb_ptr->esp0 = 0x800000-0x2000*next_process-4; //set the esp0


    if(pcb_ptr->fd_array==NULL)
    {
        check=-1;
    }
    else
    {
        check=0;
    }
    //printf("%d",check);
    if(check==-1)
    {
        strncpy(pcb_ptr->args,arg,128); //store the argument
        //cur_pcb=pcb_ptr;    //set the current pcb
        //current_process=next_process;   //set the current process
        return pcb_ptr;
    }

    int i=0;
    for(i=0;i<3;i++)
    {
        if(schedules[i]==TERMINAL_BEFORE_INITIALIZED || schedules[i]==pcb_ptr->parent_pid)
        {
            schedules[i]=pcb_ptr->pid;
            break;
        }
    }
    pcb_ptr->fd_array[0].file_operation_table_ptr=&TERMINAL_driver;  //stdin
    pcb_ptr->fd_array[0].inode=0;
    pcb_ptr->fd_array[0].file_position=0;
    pcb_ptr->fd_array[0].flags=1;

    pcb_ptr->fd_array[1].file_operation_table_ptr=&TERMINAL_driver;  //stdout
    pcb_ptr->fd_array[1].inode=0;
    pcb_ptr->fd_array[1].file_position=0;
    pcb_ptr->fd_array[1].flags=1;

    pcb_ptr->fd_array[2].flags=0;                                   //file
    pcb_ptr->fd_array[3].flags=0;                                   //file
    pcb_ptr->fd_array[4].flags=0;                                   //file
    pcb_ptr->fd_array[5].flags=0;                                   //file
    pcb_ptr->fd_array[6].flags=0;                                   //file
    pcb_ptr->fd_array[7].flags=0;                                   //file
    strncpy(pcb_ptr->args,arg,128);
    //cur_pcb=pcb_ptr;
    //current_process=next_process;
    return pcb_ptr;
}


/*
* halt
*   DESCRIPTION: halt the current process
*   INPUTS: status -- the status of the process
*   OUTPUTS: none
*   RETURN VALUE: 0
*   SIDE EFFECTS: halt the current process
*/



int32_t halt(uint8_t status)
{
    cli();
    uint32_t result=(uint32_t)status;
    uint32_t start_pid=0;
    uint32_t index_of_one_pid=0x08000000 >> 22;//index of the page directory entry
    pcb_t* curr_pcb=helper_function_of_get_curr_pcb();
    //cur_pcb = curr_pcb;
    //pcb_t* currr_pcb = cur_pcb;

    uint32_t current_pid=curr_pcb->pid;
    helper_function_of_check_exit();
    helper_function_of_close_file( );

    pid_mask[current_pid]=0;
    helper_function_of_reset_schedule(current_pid);//reset the schedule
    pcb_t* previous=(pcb_t*)(0x800000-0x2000*(curr_pcb->parent_pid+1));// get the parent pcb
    //update tss
    //tss.esp0 = parent_pcb->esp0;// restore the esp0--------------------------------------------------------------------------------------------------------------
    start_pid=curr_pcb->parent_pid;
    set_PDE_mb(index_of_one_pid,1,1,1,0,2+start_pid);//set the page directory, v128mb -> p8mb
    //flush TLB
    asm volatile(
        "movl %%cr3,%%eax     ;"
        "movl %%eax,%%cr3     ;"

        : : : "eax", "cc" 
    );
     
    tss.esp0=0x800000-0x2000*(previous->pid)-4;
    //tss.esp0=parent_pcb->esp0; 
    tss.ss0 = 0x0018;// restore the ss0 to kernel DS 
    curr_pcb=previous;//reset the pcb pointer
    int32_t ex_esp=curr_pcb->esp;
    int32_t ex_ebp=curr_pcb->ebp;
    sti();
    //ready to return
    if(status==255)
    {//exception
        result=256;
    }
    //restore registers and return result
    asm volatile(
        "movl %0, %%esp ;"
        "movl %1, %%ebp ;"
        "movl %2, %%eax ;"
        "leave;"
        "ret;"
        : 
        : "r" (ex_esp), "r" (ex_ebp), "r"(result)
        : "esp", "ebp", "eax"
    );
    return 0;//always return 0
}
/*
* The helper function of check whether the process is the initial shell
*/
void helper_function_of_check_exit()
{
    pcb_t* curr_pcb=helper_function_of_get_curr_pcb();
    if(curr_pcb->pid!=0&&curr_pcb->pid!=1&&curr_pcb->pid!=2)
    {
        return;
    }
    if(curr_pcb->pid==0)
    {//if accidentally halt the initial shell
        // sti();
        printf("Can't halt the initial shell\n");
        // current_process=-1;//reset the current process
        //---------------------------------------------------------------------------------------------------------------------------------------------
        pid_mask[curr_pcb->pid] = 0;// reset the pid mask
        execute((uint8_t*)"shell");
        //---------------------------------------------------------------------------------------------------------------------------------------------
    }
    if(curr_pcb->pid==1)
    {   
        printf("Can't halt the initial shell\n");
        pid_mask[curr_pcb->pid] = 0;// reset the pid mask
        execute((uint8_t*)"shell");
    }
    if(curr_pcb->pid==2)
    {
        printf("Can't halt the initial shell\n");
        pid_mask[curr_pcb->pid] = 0;// reset the pid mask
        execute((uint8_t*)"shell");
    }
    
}
/*
* The helper function of reset the schedule
*/
void helper_function_of_reset_schedule(int32_t current_pid)
{
    int i;
    i=0;
    while(i<3)
    {
        if(schedules[i]==current_pid)
        {
            schedules[i]=helper_function_of_get_curr_pcb()->parent_pid;
            //break;
        }
        i++;
    }
}
/*
* The helper function of close all files
*/
void helper_function_of_close_file( )
{
    int i;
    pcb_t* curr_pcb=helper_function_of_get_curr_pcb();
    i=0;
    while(i<8)
    {
        if(curr_pcb->fd_array[i].flags==1)
        {
            curr_pcb->fd_array[i].file_operation_table_ptr->close(i);
            curr_pcb->fd_array[i].flags=0;
        }
        i++;
    }
}
/*
* read
*   DESCRIPTION: read the file
*   INPUTS: fd -- file descriptor
*           buf -- buffer to store the file
*           nbytes -- number of bytes to read
*   OUTPUTS: none
*   RETURN VALUE: number of bytes read
*   SIDE EFFECTS: read the file
*/


int32_t read(int32_t fd, void* buf, int32_t nbytes)
{

    int32_t ret_val;
    pcb_t* curr_pcb = helper_function_of_get_curr_pcb();//get the current pcb
    file_descriptor_t* filedesc = curr_pcb->fd_array;
	if (fd < 0 ||
		fd >= 8 ||      // 8 is size of file array
		buf == NULL ||  // sanity check
		nbytes <= 0 ||
        fd == 1 ||      // can't read from stdout
		!(filedesc[fd].flags)) {// check whether the file is opened
		return -1;
	}

	ret_val = filedesc[fd].file_operation_table_ptr->read(fd, buf, nbytes);//call the read function
    filedesc[fd].file_position += ret_val;//update the file position
    return ret_val;

}


/*
* write
*   DESCRIPTION: write the file
*   INPUTS: fd -- file descriptor
*           buf -- buffer to store the file
*           nbytes -- number of bytes to write
*   OUTPUTS: none
*   RETURN VALUE: number of bytes written
*   SIDE EFFECTS: write the file
*/

int32_t write (int32_t fd, const void* buf, int32_t nbytes)
{
    int32_t ret_val;
    //PCB_t* pcb0 = get_cur_PCB();
    pcb_t* curr_pcb = helper_function_of_get_curr_pcb();
    file_descriptor_t* filedesc = curr_pcb->fd_array;
	if (fd <=0 ||   // can't write to stdin
		fd >= 8 ||      // 8 is size of file array
		buf == NULL ||
		nbytes <= 0 ||  // sanity check
		!(filedesc[fd].flags)) {// check whether the file is opened
		return -1;
	}

	ret_val = filedesc[fd].file_operation_table_ptr->write(fd, buf, nbytes);//call the write function
    return ret_val;
}

/*
* open
*   DESCRIPTION: open the file
*   INPUTS: filename -- name of the file
*   OUTPUTS: none
*   RETURN VALUE: file descriptor
*   SIDE EFFECTS: open the file
*/



int32_t open (const uint8_t* filename)
{
    int fd = -1;// initialize to -1 to indicate error
    int i;    // loop index
    int test;   // test whether the file exists
    dentry_t dentry;


    test = read_dentry_by_name(filename, &dentry);//read the file
    if(test == -1){
        //printf("no file");
        return -1;
    }
    // get the current pcb
    //PCB_t* cur_pcb = get_cur_PCB();
    pcb_t* curr_pcb = helper_function_of_get_curr_pcb();
    file_descriptor_t* filedesc = curr_pcb->fd_array;//get the file descriptor array
    // check whether number of opened files reach max
    for (i = 0; i < 8; i++){//looking for the first available file descriptor
        if (curr_pcb->fd_array[i].flags == 1)
            continue;
        else
            fd = i;
            break;
    }

    if (fd == -1){//no available file descriptor
        return -1;
    }

    filedesc[fd].flags = 1;//set the flag to 1
    filedesc[fd].file_position = 0;//initialize the file position
    filedesc[fd].inode = dentry.inode_num;
    if (dentry.file_type == 1){//type 1 indicates directory
        // filedesc[fd].file_operation_table_ptr->open = dopen;
        // filedesc[fd].file_operation_table_ptr->close = dclose;
        // filedesc[fd].file_operation_table_ptr->read = dread;
        // filedesc[fd].file_operation_table_ptr->write = dwrite;
        filedesc[fd].file_operation_table_ptr = &DIR_driver;//set the file operation table pointer
    }
    else if (dentry.file_type == 2){//type 2 indicates regular file
        // filedesc[fd].file_operation_table_ptr->open = fopen;
        // filedesc[fd].file_operation_table_ptr->close = fclose;
        // filedesc[fd].file_operation_table_ptr->read = fread;
        // filedesc[fd].file_operation_table_ptr->write = fwrite;
        filedesc[fd].file_operation_table_ptr = &FILE_driver;//set the file operation table pointer
    }
    else if (dentry.file_type == 0){//type 0 indicates RTC
        filedesc[fd].inode = -1;//for rtc, inode is meaningless
        // filedesc[fd].file_operation_table_ptr->open = open_rtc;
        // filedesc[fd].file_operation_table_ptr->close = close_rtc;
        // filedesc[fd].file_operation_table_ptr->read = read_rtc;
        // filedesc[fd].file_operation_table_ptr->write = write_rtc;
        filedesc[fd].file_operation_table_ptr = &RTC_driver;//set the file operation table pointer
    }
    else{
        filedesc[fd].flags = 0;//if the file type is not valid, set the flag to 0
        return -1;
    }
    filedesc[fd].file_operation_table_ptr->open(filename);//call the open function
    return fd;
}

/*
* close
*   DESCRIPTION: close the file
*   INPUTS: fd -- file descriptor
*   OUTPUTS: none
*   RETURN VALUE: 0
*   SIDE EFFECTS: close the file
*/
int32_t close (int32_t fd)
{

    //PCB_t* pcb = get_cur_PCB();
    pcb_t* curr_pcb = helper_function_of_get_curr_pcb();
    int32_t ret_val = 0;
    if (fd <= 1 || fd >= 8 || !curr_pcb->fd_array[fd].flags)//sanity check
        return -1;
    curr_pcb->fd_array[fd].file_operation_table_ptr->close(fd);//call the close function
    curr_pcb->fd_array[fd].flags = 0;//set the flag to 0
    return ret_val;
}


/*
* getargs
*   DESCRIPTION: get the argument
*   INPUTS: buf -- buffer to store the argument
*           nbytes -- number of bytes to read
*   OUTPUTS: none
*   RETURN VALUE: 0
*   SIDE EFFECTS: get the argument
*/
int32_t getargs (uint8_t* buf, int32_t nbytes){

    pcb_t* curr_pcb = helper_function_of_get_curr_pcb();
    // strncpy(buf, cur_pcb->args, nbytes);  
    if(curr_pcb==NULL || curr_pcb->args[0]=='\0' || buf==NULL || buf < (uint8_t*) 0x8000000 || nbytes<0 || (int)buf+nbytes>0x8400000-4)//sanity check
    {
        return -1;
    }
    if(nbytes>128)//max length of the argument is 128
    {
        strncpy((int8_t*)buf,curr_pcb->args,128);
    }
    else
    {
        strncpy((int8_t*)buf,curr_pcb->args,nbytes);
    }
    return 0;
}
/*
* vidmap
*   DESCRIPTION: map the video memory
*   INPUTS: screen_start -- the start of the screen
*   OUTPUTS: none
*   RETURN VALUE: 0
*   SIDE EFFECTS: map the video memory
*/

int32_t vidmap(uint8_t** screen_start){
  
    if(screen_start==NULL || screen_start < (uint8_t**) 0x8000000 || screen_start > (uint8_t**) (0x8400000-1))//sanity check
    {
        return -1;
    }
    set_PTE_map(0xB8,1,0,0xB8,1);//distribution of the video memory
    //flush TLB
    asm volatile(
        "movl %%cr3, %%eax;"
        "movl %%eax, %%cr3;"
        :
        :
        : "eax" , "cc"
    );
    *screen_start=(uint8_t*) 0x84b8000;//locate to the video memory start
    return 0;
}

/*
* set_handler
*   DESCRIPTION: set the handler
*   INPUTS: signum -- signal number
*           handler_address -- address of the handler
*   OUTPUTS: none
*   RETURN VALUE: 0
*   SIDE EFFECTS: set the handler
*/
int32_t set_handler(int32_t signum, void* handler_address){//not implemented
    return 0;
}
int32_t sigreturn(void){//not implemented
    return 0;
}

/*
* The helper function of get current pcb structure
*/

pcb_t* helper_function_of_get_curr_pcb()
{
    uint32_t curr_process;
    register int32_t esp_value asm("esp");
    curr_process=(0x800000-esp_value)/0x2000;
    pcb_t* temp=(pcb_t*)(0x800000-0x2000*(curr_process+1));
    return temp;
}
/*
* Another helper function to get current pcb
*/
pcb_t* get_pcb()
{   pcb_t* curr_pcb = helper_function_of_get_curr_pcb();
    return curr_pcb;
}
