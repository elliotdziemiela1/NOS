#pragma once
#include filesystem.h

int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
int32_t read(int32_t fd, void* buf, int32 t nbytes);
int32_t write(int32_t fd, const void* buf, int32 t nbytes);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);

typedef struct{
    file_desc_t file_array[MAX_FILES];       //contains file descriptors for files (struct file_desc_t)

} pcb_t;

typedef struct{
    //file operations jump table
    fops_t fop_func;
    //index node number for the file
    int32_t inode_num;
    int32_t file_position;
    int32_t flags;
} file_desc_t;

//file opcodes (jump) table
typedef struct{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
} fops_t;

