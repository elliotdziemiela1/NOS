#include "syscalls.h"
#include "./drivers/keyboard.h"
#include "./drivers/rtc.h"
#include "./drivers/terminal.h"
#include "./lib.h"
#include "filesystem.h"


#define JUNK 0
#define MAX_PROCESSES 10 // arbitrary number of the maximum processes we'll allow
#define EIGHT_MB 0x0800000
#define EIGHT_KB 0x0002000

uint8_t cur_pcb = 0;
static volatile pcb_t pcb_array[MAX_PROCESSES];

//table for different file operations
// fops_t fops_rtc = {rtc_read, rtc_write, rtc_open, rtc_close};
// fops_t fops_file = {read_file, write_file, open_file, close_file};
// fops_t fops_dir = {read_directory, write_directory, open_directory, close_directory};

void init_pcb(pcb_t* pcb){
    cur_pcb++;
    pcb -> pcb_id = cur_pcb;
    pcb -> parent_id = 0;
    pcb -> saved_esp = 0;
    pcb -> saved_ebp = 0;
    pcb -> active = 1;
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
    printf("initing pcb \n");
    pcb_t* pcb = get_pcb(cur_pcb+1);
    init_pcb(pcb);
    return 0;
}

int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    // printf("REACHED WRITE \n");
    pcb_t* pcb = get_pcb(cur_pcb);

    if(fd < 0 || fd > MAX_FILES){
        printf("Invalid fd \n");
        return -1;
    }
    
    // printf("calling rtc write \n");
    return pcb->file_array[fd].fops_func.write(fd, buf, nbytes); 
}

int32_t open (const uint8_t* filename){
    printf("REACHED OPEN \n");

    if(filename == NULL){
        printf("Filename empty! \n");
        return -1;
    }
    
    pcb_t* pcb = get_pcb(cur_pcb);

    // add file to fda
    int i;
    for(i = 2; i < MAX_FILES; i++){
        if(pcb->file_array[i].flags != 1)
            break;
        else if(i == MAX_FILES-1){
            printf("No free file positions \n");
            return -1;
        }
    }

    printf("i = %d \n", i);

    dentry_t dentry;
    if(read_dentry_by_name(filename, &dentry) != 0){
        printf("Dentry fail \n");
        return -1;
    }
    printf("setting fda \n");
    file_desc_t file;
    fops_t fop;

    if(strncmp(filename, "rtc", strlen(filename)) == 0){
        init_fop(&fop, 0);
        file.fops_func = fop;
        file.inode_num = 0;
        file.file_position = i;
        file.flags = 1;

        pcb->file_array[i] = file;

        printf("calling handler \n");
        pcb->file_array[i].fops_func.open(filename); 

        return i; 
    }

    init_fop(&fop, dentry.file_type);

    file.fops_func = fop;
    file.inode_num = dentry.inode_num;
    file.file_position = i;
    file.flags = 1;

    pcb->file_array[i] = file;

    printf("calling handler \n");
    pcb->file_array[i].fops_func.open(filename); 

    return i; 
}

int32_t close (int32_t fd){
    pcb_t* pcb = get_pcb(cur_pcb);

    if(fd < 0 || fd > MAX_FILES){
        printf("Invalid fd \n");
        return -1;
    }

    pcb->file_array[fd].flags = 0;

    return pcb->file_array[fd].fops_func.close(fd);
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
    pcb_t* pcb = get_pcb(cur_pcb);
    
    if(fd < 0 || fd > MAX_FILES){
        printf("Invalid fd \n");
        return -1;
    }
    
    return pcb->file_array[fd].fops_func.read(fd, buf, nbytes); 
}



