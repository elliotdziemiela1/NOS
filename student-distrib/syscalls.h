#pragma once
#include "filesystem.h"
#include "switch_to_user.h"

#define ARG_LEN 128
#define NONREADABLE      0x2 // fd flag that says it's not a file readable by cat
#define OPEN      0x1

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
    int32_t file_position; // position in file of read
    int32_t flags;
} file_desc_t;

typedef struct pcb_t{
    file_desc_t file_array[MAX_FILES];       //contains file descriptors for files (struct file_desc_t)
    uint32_t pcb_id;
    uint32_t parent_id;
    uint32_t saved_esp;
    uint32_t saved_ebp;
    uint32_t saved_eip;
    uint8_t active      :1;
    uint8_t args[ARG_LEN];
    // uint8_t active;
} pcb_t;

uint32_t get_pcb(uint8_t num);
uint32_t get_current_pcb();
void init_pcb(pcb_t* pcb, uint32_t s_esp, uint32_t s_ebp);
void init_fop(fops_t* fop, uint8_t num);
int32_t halt (uint8_t status);
int32_t execute (const uint8_t* command);
int32_t execute_elliot (const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
extern void user_switch();
int32_t dummy_read (int32_t fd, void* buf, int32_t nbytes);
int32_t dummy_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t dummy_open (int32_t fd, const void* buf, int32_t nbytes);
int32_t dummy_close (int32_t fd, const void* buf, int32_t nbytes);
// extern void flush_tlb();
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
void parse_command(const uint8_t* command, uint8_t* args, uint8_t* filename);

int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);
