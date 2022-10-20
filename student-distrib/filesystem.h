#pragma once

#define FILENAME_LENGTH 32
#define BLOCK_SIZE 4096

struct dentry{
    //name of the file which can be up to 32 characters in length
    int8_t file_name[FILENAME_LENGTH];
    //file type: 0 for user level access to RTC, 1 for directory, 2 for regular file
    int32_t file_type;
    //index node number for the file
    int32_t inode_num;
    //reserved 24B 
    int8_t reserved[24];
}dentry_t;