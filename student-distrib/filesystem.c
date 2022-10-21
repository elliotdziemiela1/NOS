#include "filesystem.h"


boot_block_t * boot_block;
inode_t * inode_start;
uint32_t first_data_block;


void initialize_filesystem(const uint32_t file_system_start_address){
    boot_block = (boot_block_t *) file_system_start_address; //boot block is first in superblock
    inode_start = (inode_t *)(file_system_start_address + BLOCK_SIZE); //start of inodes is second in super block
    first_data_block = file_system_start_address + BLOCK_SIZE + (boot_block -> num_inodes) * BLOCK_SIZE ; //data block starts after boot_block and inode blocks
}

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
    uint32_t i;
    uint32_t num_dir = boot_block -> num_directories;

    //parameter validation
    if(fname == NULL){
        return -1;
    }
    
    for(i = 0; i < num_dir; i++){
        if(strncmp((int8_t*) fname, (int8_t*) (boot_block -> direntries[i].file_name), strlen((int8_t*) fname)) == 0){
            //copy information from dentry in boot block into given dentry
            strcpy((int8_t*) dentry -> file_name, (const int8_t*) fname);
            dentry -> file_type = boot_block -> direntries[i].file_name;
            dentry -> inode_num = boot_block -> direntries[i].inode_num;
            return 0;
        }
    }
    //return -1 if unsuccessful
    return -1;   
}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
    uint32_t i;
    uint32_t num_dir = boot_block -> num_directories;

    //parameter validation
    if(index >= num_dir || index < 0 || dentry == NULL){
        return -1;
    }

    for(i = 0; i < num_dir; i++){
        if(index == boot_block -> direntries[i].inode_num){
            //copy information from dentry in boot block into given dentry
            strcpy((int8_t*) dentry -> file_name, (const int8_t*)boot_block -> direntries[i].file_name);
            dentry -> file_type = boot_block -> direntries[i].file_name;
            dentry -> inode_num = index;
            return 0;
        }
    }
    //return -1 if unsuccessful
    return -1;   
}




