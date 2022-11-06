#include "syscalls.h"
#include "./drivers/keyboard.h"
#include "filesystem.h"
#include "paging.h"

#define JUNK 0
#define MAX_PROCESSES 10 // arbitrary number of the maximum processes we'll allow
#define EXE_HEADER_SIZE 28 // the number of bytes at the beginning of an executable that contain
// magic number and entry point
#define EXE_ENTRY_PONT_OFFSET 24 // offset of entry point value in exe file header
#define EXE_MAGIC 0x464c457f // magic number at the beginning of every executible file
#define MAGIC_0 0x7f
#define MAGIC_1 0x45
#define MAGIC_2 0x4c
#define MAGIC_3 0x46
#define PROG_IMG_VIRTUAL_ADDR 0x08048000 // 128MB:virtual address we need to map physical
#define MAX_PIDS 3
#define MB_4 0x400000
// addresses of program images to (first program at 8MB physical, second program at 12MB physical)


static volatile pcb_t pcb_array[10];

uint32_t current_pid = 0;
uint32_t parent_pid = 0;
uint32_t pid_array[MAX_PIDS];


int32_t halt (uint8_t status){
    return 0;
}

/* int32_t execute (const uint8_t* command);
 * Inputs: - command = 
 * Return Value: -1 if program cannot be executed, else 256 if the program dies by an
 *  exception, or a value in the range 0 to 255 if the program executes a halt system call, 
 *  in which case the value returned is that given by the program’s call to halt.
 *  Function: 
 * */
// 1. Paging Helpers  ( optional, but very helpful )
//  Map Virtual & Physical Memory (optional, needed for CP5)
//  Unmap  Virtual & Physical Memory (optional, needed for CP5)
// 2 Parse cmd
// 3.File Checks
// 4. Create new PCB
// 5. Setup memory (aka paging)
// 6. Read exe data
// 7. Setup old stack & eip
// 8. Goto usermode
// NOTES:
//  -   executible files will be in filesystem, we just need to check them.
//  -   bit of information that you need to execute programs is the entry point into the
//      program, i.e., the virtual address of the first instruction that should be executed. 
//      This information is stored as a 4-byte unsigned integer in bytes 24-27 of the executable, 
//      and the value of it falls somewhere near 0x08048000 
int32_t execute_elliot (const uint8_t* command){
    // file exists checks
    dentry_t dent;
    if (read_dentry_by_name(command, &dent) == -1) // if file doesn't exist in file-system
        return -1; // ERROR: WHEN PASSING "ls", GETS FILE WITH NAME "rtc"
    // executible checks
    uint8_t data[EXE_HEADER_SIZE];
    if (read_data(dent.inode_num, 0, data, EXE_HEADER_SIZE) < EXE_HEADER_SIZE)
        return -1;
    if (*(uint32_t*)(data) != EXE_MAGIC)
        return -1;
    uint32_t entryPoint = ((uint32_t*)(data))[EXE_ENTRY_PONT_OFFSET/4]; /* 4 since we're casting
        as a uint32_t array*/ 
    if (entryPoint < PROG_IMG_VIRTUAL_ADDR) // can add upper bound check as well
        return -1;
    // create new PCB
    return 0;
}

int32_t execute (const uint8_t* command){
    /* Initialize Variables */
    dentry_t dentry;
    int inode;
    uint8_t * data_buffer;
    int pid_filled;
    uint32_t physical_address;

    /* Confirm file validity */
    if (read_dentry_by_name(command, &dentry) == -1) return -1;
    
    inode = dentry.inode_num;
    if(read_data(inode, 0, data_buffer, sizeof(uint32_t)) == -1) return -1;

    /* Check if file is an executable */
    if(data_buffer[0] != MAGIC_0 || data_buffer[1] != MAGIC_1 || data_buffer[2] != MAGIC_2 || data_buffer[3] != MAGIC_3){
        return -1;
    }

    /* Set up paging */
    pcb_t* pcb_ptr;
    int i;
    for(i = 0; i < MAX_PIDS; i++){ //currently only 3 PIDs
        if(pid_array[i] == 0){
            pid_array[i] = 1;
            current_pid = i;
            pid_filled = 1;
            break;
        }
    }

    //there aren't any pids available
    if(!pid_filled){
        return -1;
    }

    physical_address = (2 + current_pid) * FOURMB; //you want to skip the first page which houses the kernel
    page_dir[32].fourmb.addr = physical_address / FOURKB;

    //need to flush the TLB here 

    /* Load the file into physical memory */
    inode_t * prog_inode_ptr = (inode_t *)(inode_start + inode);
    uint8_t * img_addr = (uint8_t*) PROG_IMG_VIRTUAL_ADDR;
    read_data(inode, 0, img_addr, prog_inode_ptr -> file_size);

    /* Create PCB and Open File Descriptor*/
    

}
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    return 0;
}
int32_t open (const uint8_t* filename){
    return 0;
}
int32_t close (int32_t fd){
    return 0;
}

/* int32_t read (int32_t fd, void* buf, int32_t nbytes);
 * Inputs: - fd= file descriptor we want to call the read function of.
 *         - buf= buffer we want to fill with read bytes
 *         - nbytes= number of bytes to fill buf with
 * Return Value: returns # of bytes read
 * Function: if fd == 0, we will read from the keyboard by calling gets. Otherwise,
 *          we will call the read function given in the file descriptor fd.
 * */
// can refer to ece391hello.c for an example of a call to this function
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    if (fd != 0){
        // Should call linkage wrapper function here
        return ((file_desc_t *)fd)->fop_func.read(JUNK, buf, nbytes); 
    } else { // else read from keyboard which doesn't have a fd
        return gets(buf,nbytes);
    }
}

