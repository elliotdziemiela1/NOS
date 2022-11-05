#include "syscalls.h"
#include "./drivers/keyboard.h"
#include "./drivers/rtc.h"
#include "./lib.h"
#include "filesystem.h"


#define JUNK 0
#define MAX_PROCESSES 10 // arbitrary number of the maximum processes we'll allow
#define EIGHT_MB 0x0800000
#define EIGHT_KB 0x0002000

uint8_t num_of_pcbs = 0;
static volatile pcb_t pcb_array[MAX_PROCESSES];

//table for different file operations
// fops_t fops_rtc = {rtc_read, rtc_write, rtc_open, rtc_close};
// fops_t fops_file = {read_file, write_file, open_file, close_file};
// fops_t fops_dir = {read_directory, write_directory, open_directory, close_directory};

void init_pcb(pcb_t* pcb){
    pcb -> pcb_id = num_of_pcbs;
    pcb -> parent_id = 0;
    pcb -> saved_esp = 0;
    pcb -> saved_ebp = 0;
    pcb -> active = 1;
    
    num_of_pcbs++;
}

uint32_t get_pcb(uint8_t num){
    uint32_t addr = EIGHT_MB - EIGHT_KB*num;
    return addr;
}

void init_fop(fops_t* fop, uint8_t num){
    if(num == 0){ // rtc
        fop->read = rtc_read;
        fop->write = rtc_write;
        fop->open = rtc_open;
        fop->close = rtc_close;
        return;
    }

    if(num == 1){ // directory
        fop->read = read_file;
        fop->write = write_file;
        fop->open = open_file;
        fop->close = close_file;
        return;
    }

    if(num == 2){ // file
        fop->read = read_directory;
        fop->write = write_directory;
        fop->open = open_directory;
        fop->close = close_directory;
        return;
    }
}



int32_t halt (uint8_t status){
    return 0;
}

/* int32_t execute (const uint8_t* command);
 * Inputs: - command = 
 * Return Value: 
 * Function: 
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
int32_t execute (const uint8_t* command){
    // if (strncmp(command, {'l','s'})){

    // }
    return 0;
}

int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    printf("REACHED WRITE \n");
    pcb_t* pcb = get_pcb(1);

    // if(fd < 0 || fd > MAX_FILES || buf == NULL){
    //     return -1;
    // }
    
    printf("calling rtc write \n");
    return pcb->file_array[2].fops_func.write(fd, buf, nbytes); 
}

int32_t open (const uint8_t* filename){
    printf("REACHED OPEN \n");
    pcb_t* pcb = get_pcb(1);
    init_pcb(pcb);

    // add file to fda

    file_desc_t file;

    fops_t fop;
    init_fop(&fop, 0);

    file.fops_func = fop;
    file.inode_num = 0;
    file.file_position = 0;
    file.flags = 1;

    pcb->file_array[2] = file;


    return pcb->file_array[2].fops_func.open(filename); 









    /*

    int idx;            //index for file descriptor array
    dentry_t dentry;    //temp dentry

    //check if index in fda is available to use
    for(idx = 2; idx < MAX_FILES; idx++){          //start at index 2 because stdin and stdout
        if(pcb->file_array[idx].flags == 0){
            break;
        }
    }

    //check if file exists 
    if(read_dentry_by_name(filename, &dentry) != 0){
        return -1;
    }

    //set each function in file descriptor array based on type (rtc or file or directory) 
    if(dentry.file_type == 0){
        pcb->file_array[idx].fops_func = fops_rtc;
    }

    if(dentry.file_type == 1){
        pcb->file_array[idx].fops_func = fops_dir;
    }

    if(dentry.file_type == 2){
        pcb->file_array[idx].fops_func = fops_file;
    }

    //set file descriptor 
    pcb->file_array[idx].inode_num = dentry.inode_num;
    pcb->file_array[idx].file_position = 0;
    pcb->file_array[idx].flags = 1;
    return idx;
    */
}

int32_t close (int32_t fd){
    pcb_t* pcb = get_pcb(1);
    init_pcb(pcb);

    return 0;
}

/* read
 * Inputs: - fd= file descriptor we want to call the read function of.
 *         - buf= buffer we want to fill with read bytes
 *         - nbytes= number of bytes to fill buf with
 * Return Value: returns # of bytes read
 * Function: if fd == 0, we will read from the keyboard by calling gets. Otherwise,
 *          we will call the read function given in the file descriptor fd.
 * */
// can refer to ece391hello.c for an example of a call to this function
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    pcb_t* pcb = get_pcb(1);
    init_pcb(pcb);
    
    if(fd < 0 || fd > MAX_FILES || buf == NULL){
        return -1;
    }

    // if (fd == 0){
    //     return gets(buf,nbytes);
    // }
    
    return pcb->file_array[fd].fops_func.read(fd, buf, nbytes); 

    // if (fd != 0){
    //     // Should call linkage wrapper function here
    //     return pcb->file_array[fd].fop_func.read(fd, buf, nbytes); 
    // } else { // else read from keyboard which doesn't have a fd
    //     return gets(buf,nbytes);
    // }
}



