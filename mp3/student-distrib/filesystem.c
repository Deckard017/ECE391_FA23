# include "filesystem.h"

boot_block_t * boot_block;
inode_t * inode;
data_block_t * data_block;
dentry_t * dentry;
int32_t idx;


/*
 * file_system_init
 *   DESCRIPTION: Initializes the file system. Sets up the boot block, inode, and data block.
 *   INPUTS: start -- the start address of the file system
 *   OUTPUTS: none
 *   RETURN VALUE: 0, indicating success
 *   SIDE EFFECTS: Modifies global variables boot_block, inode_start_addr, data_block_start_addr, and dentry
 */
void file_system_init(uint32_t start){
    void* start_ptr = (void*) start;

    boot_block = (boot_block_t*) start_ptr;     //set the boot block pointer
    inode = (inode_t*) (boot_block + 1);        //set the inode pointer
    data_block = (data_block_t*) (inode  + boot_block->inode_count);        //set the data block pointer

    dentry = boot_block->dir_entries;       //set the dentry pointer
}




/*
 * read_dentry_by_name
 *   DESCRIPTION: Reads the directory entry by the given name. If the file name is too long or does not exist, return -1.
 *   INPUTS: fname -- the name of the file, dentry -- the directory entry to be read
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: Modifies the dentry pointer
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    int i;//loop counter
    // int j;//loop counter
    int file_length = 0;//length of the file name


    if(strlen((int8_t*)fname) > 32){//check if the file name is too long
        return -1;
    }
    for(i = 0; i < boot_block->dir_count; i++){//loop through all the dentries
        file_length = strlen((int8_t*)boot_block->dir_entries[i].file_name);//get the length of the file name


        if(strncmp((int8_t*)fname, (int8_t*)boot_block->dir_entries[i].file_name, 32) == 0){//check if the file name matches
            *dentry = boot_block->dir_entries[i];//copy the dentry to the dentry pointer
            // dentry->file_name[32] = '\0';//set the end of the file name to null
            return 0;
        }
    }
    return -1;
}

/*
 * read_dentry_by_index
 *   DESCRIPTION: Reads the directory entry by the given index. If the index is invalid, return -1.
 *   INPUTS: index -- the index of the file, dentry -- the directory entry to be read
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: Modifies the dentry pointer
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
    if(index >= boot_block->dir_count || index < 0){//check if index is valid
        return -1;
    }
    *dentry = boot_block->dir_entries[index];//copy the dentry to the dentry pointer
    return 0;
}

/*
 * read_data
 *   DESCRIPTION: Reads the data from the given inode at the given offset and copies it to the buffer. If the inode or offset is invalid, return 0.
 *   INPUTS: inode -- the inode of the file, offset -- the offset of the file, buf -- the buffer to copy the file content to, length -- the number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: the number of bytes read, indicating success
 *   SIDE EFFECTS: none
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    uint32_t i;//loop counter
    inode_t * inode_ptr;//inode pointer
    uint32_t bytes_read = 0;//number of bytes read
    uint32_t data_block_start;//start of the data block
    uint32_t current_block;//current block
    uint32_t current_offset;//current offset


    if(inode >= boot_block->inode_count || inode < 0){//check if inode is valid
        return 0;
    }
    inode_ptr = (inode_t*)((uint32_t)boot_block + (inode + 1) * BLOCK_SIZE);//get the inode pointer


    if(offset >= inode_ptr->length || offset < 0){//check if offset is valid
        return 0;
    }
    data_block_start = offset / BLOCK_SIZE;//get the start of the data block

    for(i = 0; i < length; i++){//loop through the length
        if(offset + i >= inode_ptr->length){//check if offset is valid
            return bytes_read;
        }
        current_block = inode_ptr->data_block_num[(offset + i )/ BLOCK_SIZE];//get the current block
        current_offset = (offset + i) % BLOCK_SIZE;//get the current offset
        // buf[i] = *((uint8_t*)((uint32_t)boot_block + (boot_block->inode_count + 1 + current_block) * BLOCK_SIZE + current_offset));//copy the data to the buffer
        buf[i] = *((uint8_t*)data_block[current_block].data + current_offset);//copy the data to the buffer
        bytes_read++;//increment the number of bytes read
    }
    return bytes_read;
}






/*
 * fopen
 *   DESCRIPTION: Open a file. If the file does not exist or is not a regular file, return -1.
 *   INPUTS: filename -- the name of the file
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t fopen(const uint8_t* filename){
    dentry_t FileDentry;
    if (read_dentry_by_name(filename, &FileDentry)){
        return -1;//file does not exist
    }

    return 0;
}

/*
 * fclose
 *   DESCRIPTION: Close a filecat. Currently not implemented, so always returns 0.
 *   INPUTS: fd -- the file descriptor of the file
 *   OUTPUTS: none
 *   RETURN VALUE: 0, indicating success
 *   SIDE EFFECTS: none
 */
int32_t fclose(int32_t fd){
    // the close will be done when file array is set to 0
    return 0;
}

/*
 * fread
 *   DESCRIPTION: Read the file data at the given file descriptor and copy its content to the buffer.
 *   INPUTS: fd -- the file descriptor of the file
 *           buf -- the buffer to copy the file content to
 *           nbytes -- the number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: the number of bytes read, indicating success
 *   SIDE EFFECTS: none
 */
int32_t fread(int32_t fd, void* buf, int32_t nbytes){

    int32_t ret_val = 0;
    // file_desc_t* file_descriptor = cur_pcb->file_descriptors;
    pcb_t* cur_pcb = get_pcb();//get the current pcb
    file_descriptor_t* file_descriptor = cur_pcb->fd_array;//get the file descriptor array
    int32_t inode = file_descriptor[fd].inode;//get the inode
    int32_t offset = file_descriptor[fd].file_position;//get the offset
    
    ret_val = read_data(inode, offset, buf, nbytes);
    return ret_val;
}

/*
 * fwrite
 *   DESCRIPTION: Write to a file. Currently not implemented, so always returns -1.
 *   INPUTS: fd -- the file descriptor of the file
 *           buf -- the buffer to write from
 *           nbytes -- the number of bytes to write
 *   OUTPUTS: none
 *   RETURN VALUE: -1, indicating failure
 *   SIDE EFFECTS: none
 */
int32_t fwrite(int32_t fd, const void* buf, int32_t nbytes){
    //readonly
    return -1;
}


/*
 * dopen
 *   DESCRIPTION: Open a directory. Currently not implemented, so always returns 0.
 *   INPUTS: fname -- the name of the directory
 *   OUTPUTS: none
 *   RETURN VALUE: 0, indicating success
 *   SIDE EFFECTS: none
 */
int32_t dopen(const uint8_t* fname){
    dentry_t DirDentry;
    if (read_dentry_by_name(fname, &DirDentry)){
        return -1;
    }
    return 0;
}

/*
 * dclose
 *   DESCRIPTION: Close a directory. Currently not implemented, so always returns 0.
 *   INPUTS: fd -- the file descriptor of the directory
 *   OUTPUTS: none
 *   RETURN VALUE: 0, indicating success
 *   SIDE EFFECTS: none
 */
int32_t dclose(int32_t fd){
    // the close will be done when file array is set to 0
    return 0;
}

/*
 * dread
 *   DESCRIPTION: Read the directory entry at the given index and copy its filename to the buffer.
 *   INPUTS: buf -- the buffer to copy the filename to
 *           idx -- the index of the directory entry to read
 *   OUTPUTS: none
 *   RETURN VALUE: The length of the filename, or -1 on failure.
 *   SIDE EFFECTS: none
 */
int32_t dread(int32_t fd,void* buf, int nbytes){
    // Initialize return value to 32, which is the maximum length of a file name
    int32_t ret_val = 32;
    // Variable to store the length of the file name
    int32_t name_length;
    // Directory entry structure to store the current directory entry
    dentry_t curdentry;
    // If reading the directory entry by index fails, return -1
    if(read_dentry_by_index(idx, &curdentry) == -1){
        idx = 0;
        return 0;
    }
    idx++;
    // Get the length of the file name
    name_length = strlen((int8_t*)(curdentry.file_name));
    // Copy the file name to the buffer, maximum length is 32
    memcpy((uint8_t*) buf, &curdentry.file_name, 32); //32 is the max of file name length
    // If the return value is greater than the length of the file name, update the return value
    if (ret_val > name_length){
        ret_val = name_length;
    }

    // Return the length of the file name
    return ret_val;
}

/*
 * dwrite
 *   DESCRIPTION: Write to a directory. Not implemented, so always returns -1.
 *   INPUTS: fd -- the file descriptor of the directory
 *           buf -- the buffer to write from
 *           nbytes -- the number of bytes to write
 *   OUTPUTS: none
 *   RETURN VALUE: -1, indicating failure
 *   SIDE EFFECTS: none
 */
int32_t dwrite(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

uint32_t get_inode_length(uint32_t inode_index){
    inode_t* inode_fxxk=inode+inode_index;
    return inode_fxxk->length;
}


