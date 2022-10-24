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
    uint32_t success_value;

    //check edge case of the file name that's greater than 32 characters

    //parameter validation
    if(fname == NULL){
        return -1;
    }
    
    for(i = 0; i < num_dir; i++){
        if(strncmp((int8_t*) fname, (int8_t*) (boot_block -> direntries[i].file_name), strlen((int8_t*) fname)) == 0){
            //copy information from dentry in boot block into given dentry
            success_value = read_dentry_by_index((uint32_t) (boot_block -> direntries[i].inode_num), (dentry_t*) dentry);
            return success_value;
        }
    }
    //return -1 if unsuccessful
    return -1;   
}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
    uint32_t num_dir = boot_block -> num_inodes;

    //parameter validation
    if(index >= num_dir || index < 0 || dentry == NULL){
        return -1;
    }

    //copy information from dentry in boot block into given dentry
    strcpy((int8_t*) dentry -> file_name, (const int8_t*) boot_block -> direntries[index].file_name);
    dentry -> file_type = boot_block -> direntries[index].file_type;
    dentry -> inode_num = index;
    return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    uint32_t inode_data_block_idx;
    uint32_t data_block_position;
    uint32_t data_block_address;
    uint32_t bytes_read;

    //parameter validation
    if(inode >= boot_block -> num_inodes || inode < 0) return -1;
    inode_t* cur_inode = (inode_t*) (&inode_start[inode]);

    if(buf == NULL){
        return -1;
    }else if(offset > (cur_inode -> file_size)){ // can't offset more than allocated space. technically reached end of file so return 0
        return 0;
    }

    inode_data_block_idx = offset / BLOCK_SIZE;
    data_block_position = offset % BLOCK_SIZE;

    data_block_address = cur_inode -> data_block_index[inode_data_block_idx];

    for(bytes_read = 0; bytes_read < length; bytes_read++, data_block_position++){
        if(data_block_position >= BLOCK_SIZE){
            data_block_position = 0;
            inode_data_block_idx += 1;
            
            data_block_address = cur_inode -> data_block_index[inode_data_block_idx];
            
            if (data_block_address >= boot_block -> num_data_blocks){
                return -1;
            }

            //reached the end of the file
            if(offset + bytes_read >= cur_inode -> file_size) break;
        }

        memcpy(buf + bytes_read, (void*)(first_data_block + (data_block_address * BLOCK_SIZE) + data_block_position), 1);
    }

    return bytes_read;
}

int32_t read_file(int32_t fd, void* buf, int32_t nbytes);
int32_t read_directory(int32_t fd, void* buf, int32_t nbytes){
    int i;
    int j;
    int inode_idx;
    int cur_file_size;
    int num_dir = boot_block -> num_directories;

    for(i = 0; i < num_dir; i++){
        dentry_t dentry = boot_block -> direntries[i];
        inode_idx = dentry.inode_num;
        cur_file_size = inode_start[inode_idx].file_size;

        if(read_dentry_by_index(i, (dentry_t*) &dentry) == -1) return -1;

        for(j = 0; j < 32; j++){
            printf("%c", dentry.file_name[j]);
        }

        printf(" File Type: %d", dentry.file_type);
        printf(" %d", cur_file_size);
        printf("\n");
    }
}

int32_t open_file(const uint32_t* filename){
    return -1;
}
int32_t write_file(int32_t fd, void* buf, int32_t nbytes){
    return -1;
}
int32_t close_file(int32_t fd){
    return -1;
}
int32_t open_directory(const uint32_t* filename){
    return -1;
}
int32_t write_directory(int32_t fd, void* buf, int32_t nbytes){
    return -1;
}
int32_t close_directory(int32_t fd){
    return -1;
}



