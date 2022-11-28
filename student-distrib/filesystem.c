#include "filesystem.h"
#include "syscalls.h"

int32_t files[MAX_FILES] = {-1, -1, -1, -1, -1, -1, -1, -1};

/* initialize_filesystem
 * 
 * Initializes addresses for file system parsing
 * Inputs: file_system_start_address -- what the name suggests
 * Outputs: None
 * Return value: None
 * Files: None
 */
void initialize_filesystem(const uint32_t file_system_start_address){
    if(file_system_start_address == NULL){
        return;
    }
    boot_block = (boot_block_t *) file_system_start_address; //boot block is first in superblock
    inode_start = (inode_t *)(file_system_start_address + BLOCK_SIZE); //start of inodes is second in super block
    first_data_block = file_system_start_address + BLOCK_SIZE + (boot_block -> num_inodes) * BLOCK_SIZE ; //data block starts after boot_block and inode blocks
}
/* read_dentry_by_name
 * 
 * Parses file system by name for specfic dentry
 * Inputs: fname -- string we want to find the dentry for
 * Outputs: dentry - dentry we found
 * Return value: 0 if succesful, -1 otherwise
 * Files: None
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
    uint32_t i;
    uint32_t num_dir = boot_block -> num_directories;
    uint32_t success_value;

    //check edge case of the file name that's greater than 32 characters
    
    //parameter validation
    if(fname == NULL || strlen((int8_t *) fname) > FILENAME_LENGTH || strlen((int8_t *) fname) == 0){
        return -1;
    }
    
    for(i = 0; i < num_dir; i++){
        if(strncmp((int8_t*) fname, (int8_t*) (boot_block -> direntries[i].file_name), strlen((int8_t*) fname)) == 0 && 
        (strlen((int8_t*) fname) == strlen((int8_t*) (boot_block -> direntries[i].file_name)))){
            //copy information from dentry in boot block into given dentry
            //success_value = read_dentry_by_index(i, (dentry_t*) dentry);
            dentry -> file_type = boot_block -> direntries[i].file_type;
            strcpy((int8_t*) dentry -> file_name, (const int8_t*) boot_block -> direntries[i].file_name);
            dentry -> inode_num = boot_block -> direntries[i].inode_num;
            //success_value = read_dentry_by_index((uint32_t) i, (dentry_t*) dentry);
            
            return success_value;
        }
    }
    //return -1 if unsuccessful
    return -1;   
}
/* read_dentry_by_idx
 * 
 * Parses file system by idx for specfic dentry
 * Inputs: index -- index num of the dentry we want
 * Outputs: dentry - dentry we found
 * Return value: 0 if succesful, -1 otherwise
 * Files: None
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
    uint32_t num_dir = boot_block -> num_inodes;

    //parameter validation
    if(index >= num_dir || index < 0 || dentry == NULL){
        return -1;
    }

    //copy information from dentry in boot block into given dentry
    dentry -> file_type = boot_block -> direntries[index].file_type;
    strcpy((int8_t*) dentry -> file_name, (const int8_t*) boot_block -> direntries[index].file_name);
    dentry -> inode_num = boot_block -> direntries[index].inode_num;
    return 0;
}

/* read_data
 * 
 * Function that parses file system, reads file from appropriate data blocks, and returns bytes to the buffer
 * Inputs: inode -- inode num of file we want to read
 *          offset -- how much we want to offset to read
 *          length -- how many bytes we want to read
 * Outputs: buf -- output buffer
 * Return value: number of bytes read from the file
 * Files: None
 */
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

    //get position to start at in data block
    inode_data_block_idx = offset / BLOCK_SIZE;
    data_block_position = offset % BLOCK_SIZE;

    data_block_address = cur_inode -> data_block_index[inode_data_block_idx];

    for(bytes_read = 0; bytes_read < length; bytes_read++, data_block_position++){
        //if we reach the end of the data block
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

        //copying byte by byte into buffer
        memcpy(buf + bytes_read, (void*)(first_data_block + (data_block_address * BLOCK_SIZE) + data_block_position), 1);
    }

    return bytes_read;
}

/* read_file
 * 
 * Reads a file if the file has been opened and puts contents in buffer
 * Inputs: fd -- file descriptor
 *         nbytes -- unused
 * Outputs: buf -- output buffer
 * Return value: number of bytes read from the file
 * Files: None
 */
int32_t read_file(int32_t fd, void* buf, int32_t nbytes){
    file_desc_t fileDesc = (((pcb_t*)get_current_pcb())->file_array[fd]);
    int32_t bytes_read = read_data(fileDesc.inode_num, fileDesc.file_position, (uint8_t *) buf, nbytes);
    return bytes_read;
}

/* read_directory
 * 
 * Prints files and file information to the terminal
 * Inputs: fd, buf, nbytes (all of these are unused and just a formality)
 * Outputs: Prints value to the screen
 * Return value: 0 if successul, -1 otherwise
 * Files: None
 */
int32_t read_directory(int32_t fd, void* buf, int32_t nbytes){
    dentry_t dentry;
    pcb_t * cur_pcb = (pcb_t*) get_current_pcb();
    int32_t count = cur_pcb -> file_array[fd].file_position;

    if(read_dentry_by_index(count, &dentry) == -1){
        return -1;
    }

    strncpy((int8_t*) buf, (int8_t*) & (dentry.file_name), FILENAME_LENGTH);
    int32_t bytes_read = strlen((int8_t*) & (dentry.file_name));

    count+=1;
    cur_pcb -> file_array[fd].file_position = count;

    if(bytes_read > FILENAME_LENGTH){
        return FILENAME_LENGTH;
    }

    return bytes_read;

}

/* open_file
 * 
 * Adds file to the file array
 * Inputs: file name
 * Outputs: none
 * Return value: 0 if successul, -1 otherwise
 * Files: None
 */
int32_t open_file(const uint8_t* filename){
    if(filename == NULL){
        return -1;
    }

    dentry_t dentry;
    if (read_dentry_by_name(filename, (dentry_t*) &dentry) == -1){
        return -1; //file does not exist
    }

    int inode_num = dentry.inode_num;
    int i;
    for(i = FILE_START_IDX; i < MAX_FILES; i++){
        if(files[i] == inode_num){
            return 0; //file is already open
        }
    }
    for(i = FILE_START_IDX; i < MAX_FILES; i++){
        if(files[i] == -1){
            files[i] = inode_num;
            return 0; //file was opened
        }
    }

    //if we're at this point, there are already five files open
    return -1;
}

/* close_file
 * 
 * Removes open file from file array
 * Inputs: file name
 * Outputs: none
 * Return value: 0 if successul, -1 otherwise
 * Files: None
 */
int32_t close_file(int32_t filename){
    //int i;
    // dentry_t dentry;
    // read_dentry_by_name(filename, (dentry_t*) &dentry);
    // int32_t inode_num = dentry.inode_num;
    // for(i = FILE_START_IDX; i < MAX_FILES; i++){
    //     if(inode_num == files[i]){
    //         files[i] = -1;
    //         return 0;
    //     }
    // }
    return 0;
}

/* get_file_length
 * 
 * Function returns file length
 * Inputs: node number
 * Outputs: None
 * Return value: length of file
 * Files: None
 */
int32_t get_file_length(int32_t inode_num){
    return inode_start[inode_num].file_size;
}

/* write_file
 * 
 * Function changes frequency and writes a file 
 * Inputs: file descriptor, buffer to read, number of bytes to read
 * Outputs: None
 * Return value: always -1 since file is read only
 * Files: None
 */
int32_t write_file(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/* open_directory
 * 
 * Function provides access to file system
 * Inputs: file name
 * Outputs: None
 * Return value: -1
 * Files: None
 */
int32_t open_directory(const uint8_t* filename){
    return -1;
}

/* write_directory
 * 
 * Function writes to directory
 * Inputs: file descriptor, buffer to read, number of bytes to read
 * Outputs: None
 * Return value: always -1 since file is read only
 * Files: None
 */
int32_t write_directory(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/* close_directory
 * 
 * Function closes the specified file descriptor
 * Inputs: file descriptor, buffer to read, number of bytes to read
 * Outputs: None
 * Return value: always -1 since file is read only
 * Files: None
 */
int32_t close_directory(int32_t fd){
    return -1;
}



