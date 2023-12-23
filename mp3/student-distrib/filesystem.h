# ifndef FILESYS_H
# define FILESYS_H
#include "types.h"
#include "lib.h"
#include "syscall.h"
# define BLOCK_SIZE 4096

typedef struct dentry_t{
    int8_t file_name[32];
    int32_t file_type;
    int32_t inode_num;
    int8_t reserved[24];
} dentry_t;

typedef struct boot_block_t{
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    int8_t reserved[52];
    dentry_t dir_entries[63];
} boot_block_t;

typedef struct inode_t{
    int32_t length;
    int32_t data_block_num[1023];
} inode_t;

typedef struct data_block_t{
    int8_t data[4096];
} data_block_t;


void file_system_init(uint32_t start);
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

int32_t fopen(const uint8_t* filename);
int32_t fclose(int32_t fd);
int32_t fread(int32_t fd, void* buf, int32_t nbytes);
int32_t fwrite(int32_t fd, const void* buf, int32_t nbytes);
int32_t dopen(const uint8_t* fname);
int32_t dclose(int32_t fd);
int32_t dread(int32_t fd,void* buf, int nbytes);
int32_t dwrite(int32_t fd, const void* buf, int32_t nbytes);


uint32_t get_inode_length(uint32_t inode_index);

# endif
