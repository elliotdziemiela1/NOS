#pragma once
#include "filesystem.h"

//file opcodes (jump) table
typedef struct fops_t{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
} fops_t;


typedef struct file_desc_t{
    //file operations jump table
    fops_t fops_func;
    //index node number for the file
    int32_t inode_num;
    int32_t file_position;
    int32_t flags;
} file_desc_t;

typedef struct pcb_t{
    file_desc_t file_array[MAX_FILES];       //contains file descriptors for files (struct file_desc_t)
    uint32_t pcb_id;
    uint32_t parent_id;
    uint32_t saved_esp;
    uint32_t saved_ebp;
    uint8_t active      :1;
    // uint8_t active;
} pcb_t;

uint32_t get_pcb(uint8_t num);
void init_pcb(pcb_t* pcb);
void init_fop(fops_t* fop, uint8_t num);
int32_t halt (uint8_t status);
int32_t execute (const uint8_t* command);
int32_t execute_elliot (const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
