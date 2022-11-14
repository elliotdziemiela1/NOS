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
#define MAX_PIDS 6
#define MB_4 0x400000

#define MB_128_PAGE 32
#define ELF_SIZE 4
#define PROG_IMG_START_BYTE 24

page_table_entry_t video_page[num_pte] __attribute__((aligned(pte_size)));

// addresses of program images to (first program at 8MB physical, second program at 12MB physical)

int32_t current_pid = -1; // -1 means no current process
int32_t parent_pid = -1; // -1 means no parent process
uint32_t pid_array[MAX_PIDS] = { 0 }; // array of flags telling us which PIDS are availible
uint32_t inode_array[MAX_PIDS]= { 0 }; // array of inodes of the executible files indexed in order
// of PIDS

uint8_t user_eip[ELF_SIZE]; // buffer holding the entry point for the program.

/* uint32_t get_ebp;
 * Inputs: - pid= process id to get ebp of
 * Return Value: returns the ebp of the process with id pid
 * */
uint32_t get_ebp(uint8_t pid){
    uint32_t addr = EIGHT_MB - EIGHT_KB*(pid);
    return addr;
}

/* void init_fop;
 * Inputs: - fop= pointer to file operations structure we want to initialize
 *         - num= corresponds to the type of file to init fops to
 * Return Value: none
 * Function: initializes the fop structure depending on which file type it is given by num.
 * */
void init_fop(fops_t* fop, uint8_t num){
    if(num == 0){ // rtc
        fop->read = rtc_read;
        fop->write = rtc_write;
        fop->open = rtc_open;
        fop->close = rtc_close;
        return;
    }

    if(num == 1){ // directory
        fop->read = read_directory;
        fop->write = write_directory;
        fop->open = open_directory;
        fop->close = close_directory;
        return;
    }

    if(num == 2){ // file
        fop->read = read_file;
        fop->write = write_file;
        fop->open = open_file;
        fop->close = close_file;
        return;
    }

    if(num == 3){ // std_in
        fop->read = terminal_read;
        fop->write = dummy_write;
        fop->open = NULL;
        fop->close = NULL;
        return;
    }

    if(num == 4){ // std_out
        fop->read = dummy_read;
        fop->write = terminal_write;
        fop->open = NULL;
        fop->close = NULL;
        return;
    }

    if(num == 5){ // executible
        fop->read = NULL;
        fop->write = NULL;
        fop->open = NULL;
        fop->close = NULL;
        return;
    }
}

/* void init_pcb;
 * Inputs: - pcb= pointer to pcb stuct to initialize
 * Return Value: void
 * Function: initializes pcb members. Initializes first 2 entries of file array with 
 *  stdin and stdout
 * */
void init_pcb(pcb_t* pcb){
    pcb -> pcb_id = current_pid;
    pcb -> parent_id = parent_pid;
    register uint32_t  s_esp asm("esp");
    register uint32_t  s_ebp asm("ebp");
    pcb -> saved_esp = s_esp; // saves current esp
    pcb -> saved_ebp = s_ebp; // saves current ebp
    pcb -> active = 1; // sets active
    pcb -> args[0] = '\n';

    // setting stdin file
    file_desc_t file;
    fops_t fop;
    init_fop(&fop, 3);
    file.fops_func = fop;
    file.inode_num = 0;
    file.file_position = 0;
    file.flags = OPEN;
    file.flags |= NONREADABLE;
    pcb->file_array[0] = file; 

    // setting stdout file
    init_fop(&fop, 4);
    file.fops_func = fop;
    file.inode_num = 0;
    file.file_position = 0;
    file.flags = OPEN;
    file.flags |= NONREADABLE;
    pcb->file_array[1] = file; 
}

/* uint32_t get_pcb;
 * Inputs: - pid= process id to get pcb of
 * Return Value: pointer to pcb of process
 * */
uint32_t get_pcb(uint8_t pid){
    uint32_t addr = EIGHT_MB - EIGHT_KB*(pid+1); // +1 since PCB starts at top of 8kB chunk
    return addr;
}

/* uint32_t halt;
 * Inputs: - status= status upon which halt was called
 * Return Value: returns the status upon which the halt was called
 * Function: terminates a process, returning the specified value to its parent process. 
 *  resets ebp and esp to parent's, and changes current and parrent PIDs and corresponding arrays.
 * */
int32_t halt (uint8_t status){

    pcb_t* cur_pcb = (pcb_t*) get_pcb(current_pid);
    int32_t old_esp = cur_pcb->saved_esp;
    int32_t old_ebp = cur_pcb->saved_ebp;

    pid_array[current_pid] = 0;

    if (parent_pid == -1){ // if we have only 1 running process or none at all
        current_pid = -1;
        execute((const uint8_t*)"shell"); // set flag to restart shell at end
    }

    // change current and parent pid variables, and set current pid array value to 0
    pcb_t* parent_pcb = (pcb_t*) get_pcb(parent_pid);
    current_pid = parent_pid;
    parent_pid = parent_pcb->parent_id;

    //restore parent paging
    uint32_t physical_address = (2 + current_pid) * FOURMB; //you want to skip the first page which houses the kernel
    page_dir[MB_128_PAGE].fourmb.addr = physical_address / FOURKB;
    //flush tlb; takes place after change in paging structure
    asm volatile("\
    mov %cr3, %eax    ;\
    mov %eax, %cr3    ;\
    ");
    /* Load the file into physical memory */
    inode_t * prog_inode_ptr = (inode_t *)(inode_start + inode_array[current_pid]); // somehow need to get parent inode
    uint8_t * img_addr = (uint8_t*) PROG_IMG_VIRTUAL_ADDR;
    read_data(inode_array[current_pid], 0, img_addr, prog_inode_ptr -> file_size);
    

    //close any relevant FDs
    int i, temp;
    for(i = 0; i<MAX_FILES; i++){
        temp = (int) cur_pcb->file_array[i].fops_func.close; // GDB not going into this
    }

    read_data(inode_array[current_pid], 24, user_eip, ELF_SIZE); // changes eip to parent program
    // restore esp and ebp
    asm volatile ("                 \n\
            movl    %1, %%esp  #restore esp     \n\
            movl    %2, %%ebp  #restore ebp     \n\
            # movl    $0, %%eax  #clear eax     \n\
            # movb    %0, %%al   #set return value     \n\
            # ret                     \n\
            "
            :
            : "r"(status), "r"(old_esp), "r"(old_ebp)
    );
    user_switch();
    return 0;
}

/* int32_t execute (const uint8_t* command);
 * Inputs: - command = filename string
 * Return Value: -1 if program cannot be executed, else 256 if the program dies by an
 *  exception, or a value in the range 0 to 255 if the program executes a halt system call, 
 *  in which case the value returned is that given by the program’s call to halt.
 * Function: 
 * */
int32_t execute (const uint8_t* command){
    // printf("Reached exec \n");
    /* Initialize Variables */
    dentry_t dentry;
    int inode;
    uint8_t data_buffer[ELF_SIZE] = { '\0' };
    uint32_t physical_address;
    int32_t user_esp;
    uint8_t args[ARG_LEN] = { '\0' }; //store args extracted from command parameter
    uint8_t filename[ARG_LEN] = { '\0' }; //store file name extracted from command parameter

    //there aren't any pids available
    if(current_pid == MAX_PIDS - 1){
        return -1;
    }
    //parse command and store args in args[]
    parse_command(command, args, filename);

    /* Confirm file validity */
    if (read_dentry_by_name(filename, &dentry) == -1) return -1;
    
    inode = dentry.inode_num;
    if(read_data(inode, 0, data_buffer, sizeof(uint32_t)) == -1) return -1;

    /* Check if file is an executable */
    if(data_buffer[0] != MAGIC_0 || data_buffer[1] != MAGIC_1 || data_buffer[2] != MAGIC_2 || data_buffer[3] != MAGIC_3){ //checks each specific byte
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
            break;
        }
    }
    inode_array[current_pid] = inode;

    // printf("setting physical addr \n");
    physical_address = (2 + current_pid) * FOURMB; //you want to skip the first page which houses the kernel
    page_dir[MB_128_PAGE].fourmb.addr = physical_address / FOURKB;
    page_dir[MB_128_PAGE].fourmb.present = 1; // marks as present
    page_dir[MB_128_PAGE].fourmb.rw = 1; // allows writing as well
    page_dir[MB_128_PAGE].fourmb.ps = 1; // sets page size
    page_dir[MB_128_PAGE].fourmb.su = 1; // sets supervisor bit

    //need to flush the TLB here 
    asm volatile("\
    mov %cr3, %eax    ;\
    mov %eax, %cr3    ;\
    ");

    // printf("load file into physical memory \n");
    /* Load the file into physical memory */
    inode_t * prog_inode_ptr = (inode_t *)(inode_start + inode);
    uint8_t * img_addr = (uint8_t*) PROG_IMG_VIRTUAL_ADDR;
    read_data(inode, 0, img_addr, prog_inode_ptr -> file_size);

    /* Create PCB and Open File Descriptor*/

    /* Prepare for the context switch */
    
    //Parse User EIP
    read_data(dentry.inode_num, PROG_IMG_START_BYTE, user_eip, ELF_SIZE);
    
    // printf("parse user ESP \n");
    //Parse User ESP
    user_esp = 0x083FFFFC; //this is the user esp

    // tss
    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHT_MB - EIGHT_KB*(current_pid+1) - 4; //we subtract 4 cause we don't want the top of the page

    pcb_t* pcb_ptr = (pcb_t*) get_pcb(current_pid);
    init_pcb(pcb_ptr);

    //store arg in pcb
    // strncpy((int8_t*)(pcb_ptr->args), (int8_t*)args, ARG_LEN);
    strncpy((int8_t*)(pcb_ptr->args), (int8_t*)args, ARG_LEN);

    user_switch();

    return 0;
}

//need to account for leading spaces
//command = execute paramter consisting of filename and args
//args = array to store args from parameter
//filename = array to store filename (first word) from paramter + NULL char
void parse_command(const uint8_t* command, uint8_t* args, uint8_t* filename){
    //extract filename
    int i = 0; //counter for filename and command
    int name_end; 
    while(command[i] != ' ' && command[i] != '\0'){
        filename[i] = command[i];
        i++;
    }

    if(command[i] == ' '){
        filename[i] = '\0';     //terminate command
        i++;
        name_end = i;           
    }

    //extract args
    while(command[i] != '\0'){
        if(i - name_end < ARG_LEN){
            args[i-name_end] = command[i];
            i++;
        }
    }
    // args[i - name_end] = '\0';

}


/* int32_t write (int32_t fd, const void* buf, int32_t nbytes);
 * Inputs: - fd = file descriptor number, buf = buffer, nbytes = number of bytes written
 * Return Value: -1 if program cannot be executed else value of corresponding file operation
 * Function: writes to the buffer
 * */
int32_t write (int32_t fd, const void* buf, int32_t nbytes){

    if(fd < 0 || fd > MAX_FILES){
        printf("Invalid fd \n");
        return -1;
    }
    pcb_t* pcb = (pcb_t*) get_pcb(current_pid);

    if(!(pcb->file_array[fd].flags & OPEN)){
        printf("Can't close unopened \n");
        return -1;
    }
    
    // printf("calling rtc write \n");
    return pcb->file_array[fd].fops_func.write(fd, buf, nbytes); 
}

/* int32_t open (const uint8_t* filename);
 * Inputs: - filename - string that has filename to be opened
 * Return Value: -1 if program cannot be executed else value of corresponding file operation
 * Function: opens file 
 * */
int32_t open (const uint8_t* filename){
    // printf("Reached system open \n");

    if(filename == NULL){
        printf("Filename empty! \n");
        return -1;
    }
    dentry_t dentry;
    if(read_dentry_by_name(filename, &dentry) == -1){ // -1 is failure
        return -1;
    }
    
    pcb_t* pcb = (pcb_t*) get_pcb(current_pid);

    // add file to fda
    int i;
    for(i = 2; i < MAX_FILES; i++){
        if(!(pcb->file_array[i].flags & OPEN))
            break;
        else if(i == MAX_FILES-1){
            printf("No free file positions \n");
            return -1;
        }
    }

    file_desc_t file;
    fops_t fop;

    if(strncmp((const int8_t *) filename, "rtc", strlen( (const int8_t *)filename)) == 0){
        init_fop(&fop, 0);
        file.fops_func = fop;
        file.inode_num = 0;
        file.file_position = 0;
        file.flags = OPEN;
        file.flags |= NONREADABLE;

        pcb->file_array[i] = file;

        pcb->file_array[i].fops_func.open(filename); 

        return i; 
    }

    init_fop(&fop, dentry.file_type);

    file.fops_func = fop;
    file.inode_num = dentry.inode_num;
    file.file_position = 0;
    file.flags = OPEN;

    uint8_t data_buffer[ELF_SIZE] = { '\0' };
    read_data(dentry.inode_num, 0, data_buffer, sizeof(uint32_t));

    if (dentry.file_type != 2){ // if nonfile
        file.flags |= NONREADABLE;
    } else if (data_buffer[0] == MAGIC_0 && data_buffer[1] == MAGIC_1 &&
     data_buffer[2] == MAGIC_2 && data_buffer[3] == MAGIC_3){ // if executible
        file.flags |= NONREADABLE;
        init_fop(&fop, 5); // 5 for executible
    }

    pcb->file_array[i] = file;

    pcb->file_array[i].fops_func.open(filename); 

    return i; 
}

/* int32_t open (int32_t fd);
 * Inputs: - fd - file descirptor number for pcb
 * Return Value: -1 if program cannot be executed else value of corresponding file operation
 * Function: closes file 
 * */
int32_t close (int32_t fd){
    if(fd < 2 || fd > MAX_FILES){
        printf("Invalid fd \n");
        return -1;
    }



    pcb_t* pcb = (pcb_t*) get_pcb(current_pid);
    if(!(pcb->file_array[fd].flags & OPEN)){
        printf("Can't close unopened \n");
        return -1;
    }
    pcb->file_array[fd].flags ^= OPEN; // closes file
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
    // printf("Reached open \n");
    //printf("fd = %d \n",fd);
    sti();
    if(fd < 0 || fd > MAX_FILES){
        printf("Invalid fd \n");
        return -1;
    }
    // printf("Reached system read \n");
    pcb_t* pcb = (pcb_t*) get_pcb(current_pid);
    
    if(!(pcb->file_array[fd].flags & OPEN)) // not opened
        // printf("finished open \n");
        return -1;
    if (pcb->file_array[fd].flags & NONREADABLE){
        // printf("finished open \n");
        return pcb->file_array[fd].fops_func.read(pcb->file_array[fd].inode_num, buf, nbytes);
    } else {
        // printf("finished open \n");
        if(pcb->file_array[fd].file_position >= inode_start[pcb->file_array[fd].inode_num].file_size) // if read past end
            return 0;
        int32_t ret = pcb->file_array[fd].fops_func.read(pcb->file_array[fd].inode_num, buf, nbytes);
        pcb->file_array[fd].file_position += ret;
        return ret;
    }
}

int32_t dummy_read (int32_t fd, void* buf, int32_t nbytes){
    return -1;
}

int32_t dummy_write (int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

int32_t getargs (uint8_t* buf, int32_t nbytes){
    // int i;
    pcb_t* pcb_ptr = (pcb_t*) get_pcb(current_pid);

    //check if buffer is null and if argument fits in buffer
    if(buf == NULL || (strlen((int8_t*)pcb_ptr->args) > nbytes)){
        return -1;
    }
    
    //check if args exist
    if(pcb_ptr->args[0] == '\0'){
        return -1;
    }
    // for(i = 0; i < nbytes; i++){
    //     buf[i] = pcb_ptr->args[i];
    // }

    //fill buffer with arg (kernel to user)
    strncpy((int8_t*)buf, (int8_t*)pcb_ptr->args, nbytes);

    return 0;
}

int32_t vidmap (uint8_t** screen_start){
    if(screen_start == NULL){
        return -1;
    }

    //check if address falls in address range (128 MB to 132 MB)
    if((int) screen_start < (FOURMB * 32) || (int) screen_start >= (FOURMB*32) + FOURMB){
        return -1;
    }

    // //make page table similar to the one already in paging files? 
    // int i;
    // for(i = 0; i < num_pte; i++){
    //     video_page[i].addr = i;
    //     video_page[i].present = 1;
    //     video_page[i].su = 1;      //1 for user
    //     video_page[i].rw = 1;

    //     video_page[i].pat = 0;
    //     video_page[i].g = 0;

    //     video_page[i].dirty = 0;
    //     video_page[i].a = 0;

    //     video_page[i].pcd = 0;
    //     video_page[i].pwt = 0;
    // }

    //set pointer of start of video mem address to 132 MB
    *screen_start = (uint8_t)((FOURMB*32) + FOURMB); 

    return 0;
}

int32_t set_handler (int32_t signum, void* handler_address){
    return -1;
}

int32_t sigreturn (void){
    return -1;
}

