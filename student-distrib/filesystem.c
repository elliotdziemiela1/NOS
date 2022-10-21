#include "filesystem.h"

boot_block_t * boot_block;
inode_t * inode_start;
uint32_t data_block_first;


void initialize_filesystem(const uint32_t file_system_start_address){
    boot_block = (boot_block_t *) file_system_start_address;
    inode_start = (inode_t * )(file_system_start_address + BLOCK_SIZE);
    data_block_first = file_system_start_address + (boot_block -> num_inodes) * BLOCK_SIZE + BLOCK_SIZE;
};

