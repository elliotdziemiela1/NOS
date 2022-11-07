#include "syscalls.h"
#include "./drivers/keyboard.h"
#include "./drivers/rtc.h"
#include "./drivers/terminal.h"
#include "./lib.h"
#include "filesystem.h"
#include "paging.h"
#include "x86_desc.h"



#define JUNK 0
#define MAX_PROCESSES 10 // arbitrary number of the maximum processes we'll allow
#define EIGHT_MB 0x0800000
#define EIGHT_KB 0x0002000

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

uint32_t current_pid = 0;
uint32_t parent_pid = 0;
uint32_t pid_array[MAX_PIDS];

uint8_t user_eip[4];


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

    if(num == 3){ // std_in
        fop->read = terminal_read;
        fop->write = NULL;
        fop->open = NULL;
        fop->close = NULL;
        return;
    }

    if(num == 4){ // std_out
        fop->read = NULL;
        fop->write = terminal_write;
        fop->open = NULL;
        fop->close = NULL;
        return;
    }
}

void init_pcb(pcb_t* pcb){
    pcb -> pcb_id = current_pid;
    pcb -> parent_id = parent_pid;
    register uint32_t  s_esp asm("esp");
    register uint32_t  s_ebp asm("ebp");
    pcb -> saved_esp = s_esp;
    pcb -> saved_ebp = s_ebp;
    pcb -> active = 1;

    // setting stdin
    file_desc_t file;
    fops_t fop;
    init_fop(&fop, 3);
    file.fops_func = fop;
    file.inode_num = 0;
    file.file_position = 0;
    file.flags = 1;
    pcb->file_array[0] = file; 

    // setting stdout
    init_fop(&fop, 4);
    file.fops_func = fop;
    file.inode_num = 0;
    file.file_position = 1;
    file.flags = 1;
    pcb->file_array[1] = file; 
}

uint32_t get_pcb(uint8_t num){
    uint32_t addr = EIGHT_MB - EIGHT_KB*(num+1);
    return addr;
}

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
int32_t execute (const uint8_t* command){
    // printf("Reached exec \n");
    /* Initialize Variables */
    dentry_t dentry;
    int inode;
    uint8_t data_buffer[4];
    int pid_filled;
    uint32_t physical_address;
    uint8_t user_eip_buf[4];
    int32_t user_esp;

    /* Confirm file validity */
    if (read_dentry_by_name(command, &dentry) == -1) return -1;
    
    inode = dentry.inode_num;
    if(read_data(inode, 0, data_buffer, sizeof(uint32_t)) == -1) return -1;

    /* Check if file is an executable */
    if(data_buffer[0] != MAGIC_0 || data_buffer[1] != MAGIC_1 || data_buffer[2] != MAGIC_2 || data_buffer[3] != MAGIC_3){
        return -1;
    }

    parent_pid = current_pid;
    // printf("Setting up paging \n");
    /* Set up paging */
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

    // printf("setting physical addr \n");
    physical_address = (2 + current_pid) * FOURMB; //you want to skip the first page which houses the kernel
    page_dir[32].fourmb.addr = physical_address / FOURKB;
    page_dir[32].fourmb.present = 1; // marks as present
    page_dir[32].fourmb.rw = 1; // allows writing as well
    page_dir[32].fourmb.ps = 1; // sets page size
    page_dir[32].fourmb.su = 1; // sets supervisor bit

    // printf("flushing tlb \n");
    //need to flush the TLB here 
    //flush_tlb();
    asm volatile("\
    mov %cr3, %eax    ;\
    mov %eax, %cr3    ;\
    ");

    // printf("load file into physical memory \n");
    /* Load the file into physical memory */
    inode_t * prog_inode_ptr = (inode_t *)(inode_start + inode);
    uint8_t * img_addr = PROG_IMG_VIRTUAL_ADDR;
    read_data(inode, 0, img_addr, prog_inode_ptr -> file_size);

    /* Prepare for the context switch */
    
    // printf("parse user EIP \n");
    //Parse User EIP
    read_data(dentry.inode_num, 24, user_eip, 4);
    // for(i = 3; i >= 0; i--){
    //     user_eip += user_eip_buf[i];
    //     user_eip << 8;
    // }
    
    // printf("parse user ESP \n");
    //Parse User ESP
    user_esp = 0x083FFFFC;

    // printf("changing tss \n");

    // tss
    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHT_MB - EIGHT_KB*(current_pid+1) - 4;

    // printf("creating PCB \n");
    /* Create PCB and Open File Descriptor*/
    // printf("Creating pcb \n");
    pcb_t* pcb_ptr = get_pcb(current_pid);
    init_pcb(pcb_ptr);

    // printf("setting interrupts \n");
    //switching to user mode

    // printf("context switch \n");
    // asm volatile ("\
    // andl $0x00FF, %%eax ;\
    // movw %%ax, %%dx     ;\
    // pushl %%eax         ;\
    // pushl %%edx         ;\
    // pushfl              ;\
    // popl %%eax          ;\
    // orl $0x0200, %%eax  ;\
    // pushl %%eax         ;\
    // pushl %%ecx         ;\
    // pushl %%ebx         ;\
    // iret                ;\
    // "
    // :
    // : "a"(USER_DS), "b"(user_eip), "c"(USER_CS), "d"(user_esp)
    // : "memory"
    // );
    user_switch();

    return 0;
}

int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    pcb_t* pcb = get_pcb(current_pid);

    if(fd < 0 || fd > MAX_FILES){
        printf("Invalid fd \n");
        return -1;
    }
    
    // printf("calling rtc write \n");
    return pcb->file_array[fd].fops_func.write(fd, buf, nbytes); 
}

int32_t open (const uint8_t* filename){
    printf("Reached system open \n");

    if(filename == NULL){
        printf("Filename empty! \n");
        return -1;
    }
    
    pcb_t* pcb = get_pcb(current_pid);

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
    file_desc_t file;
    fops_t fop;

    if(strncmp(filename, "rtc", strlen(filename)) == 0){
        init_fop(&fop, 0);
        file.fops_func = fop;
        file.inode_num = 0;
        file.file_position = i;
        file.flags = 1;

        pcb->file_array[i] = file;

        pcb->file_array[i].fops_func.open(filename); 

        return i; 
    }

    init_fop(&fop, dentry.file_type);

    file.fops_func = fop;
    file.inode_num = dentry.inode_num;
    file.file_position = i;
    file.flags = 1;

    pcb->file_array[i] = file;

    pcb->file_array[i].fops_func.open(filename); 

    return i; 
}

int32_t close (int32_t fd){
    printf("Reached system close");
    pcb_t* pcb = get_pcb(current_pid);

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
    sti();
    printf("Reached system read \n");
    pcb_t* pcb = get_pcb(current_pid);
    printf("Received new pcb \n");
    if(fd < 0 || fd > MAX_FILES){
        printf("Invalid fd \n");
        return -1;
    }
    
    return pcb->file_array[fd].fops_func.read(fd, buf, nbytes); 
}



