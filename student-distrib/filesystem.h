#pragma once
#include "lib.h"
#include "types.h"

#define FILENAME_LENGTH 32
#define BLOCK_SIZE 4096
#define BLOCK_NUM 63 
#define MAX_DATA_BLOCK_INDEX 1023
#define MAX_DENTRIES

typedef struct dentry{
    //name of the file which can be up to 32 characters in length
    int8_t file_name[FILENAME_LENGTH];
    //file type: 0 for user level access to RTC, 1 for directory, 2 for regular file
    int32_t file_type;
    //index node number for the file
    int32_t inode_num;
    //reserved 24B 
    int8_t reserved[24];
}dentry_t;

//this is the first dentry inside the directory
typedef struct boot_block{
    int32_t num_directories;
    int32_t num_inodes;
    int32_t num_data_blocks;
    int8_t reserved[52];
    dentry_t direntries[63];
}boot_block_t;

typedef struct inode{
    int32_t file_size;
    int32_t data_block_index[MAX_DATA_BLOCK_INDEX];
}inode_t;

void initialize_filesystem(const uint32_t file_system_start_address);

//file_system_utility_functions
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);


