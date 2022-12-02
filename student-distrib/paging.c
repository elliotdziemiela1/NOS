#include "paging.h"

// notes:
// - first 10 bits of virtual address are index into page_dir array
// - next 10 bits are index into PDEs page_table array
// - next 12 bits are offset into page selected from PTE

// the index to insert the next PDE/4MB page.

/* init_paging
 * 
 * initializes page directory and video memory (page table)
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging init
 * Files: 
 */
void init_paging(){
    // page table setup
    int i;
    for(i = 0; i < num_pte; i++){
        video_mem[i].addr = i;
        video_mem[i].present = 0;
        video_mem[i].su = 0; // sets to supervisor
        video_mem[i].rw = 1;

        video_mem[i].pat = 0;
        video_mem[i].g = 0;

        video_mem[i].dirty = 0;
        video_mem[i].a = 0;

        video_mem[i].pcd = 0;
        video_mem[i].pwt = 0;
    }
    for(i = 0; i < num_pte; i++){
        video_mem_map[i].addr = i;
        video_mem_map[i].present = 0;
        video_mem_map[i].su = 0; 
        video_mem_map[i].rw = 1;

        video_mem_map[i].pat = 0;
        video_mem_map[i].g = 0;

        video_mem_map[i].dirty = 0;
        video_mem_map[i].a = 0;

        video_mem_map[i].pcd = 0;
        video_mem_map[i].pwt = 0;
    }

    // set video memory
    uint32_t idx = VIDEO >> add_shift; // masks top 20 bits of addr
    video_mem[idx].present = 1; // marks as present
    video_mem[idx+1].present = 1; // video page for terminal 1
    video_mem[idx+2].present = 1; // video page for terminal 2
    video_mem[idx+3].present = 1; // video page for terminal 3
    

    // page directory setup
    for(i = 0; i < num_pde; i++){
        // init 4 kb
        page_dir[i].fourkb.addr = 0x0000;
        page_dir[i].fourkb.present = 0;
        page_dir[i].fourkb.su = 0; // sets to kernel
        page_dir[i].fourkb.rw = 1;

        page_dir[i].fourkb.g = 0;

        page_dir[i].fourkb.ps = 0;
        page_dir[i].fourkb.a = 0;
        page_dir[i].fourkb.avl = 0;

        page_dir[i].fourkb.pcd = 0;
        page_dir[i].fourkb.pwt = 0;
    }

    // init 4 kb
    page_dir[0].fourkb.addr = (int)(video_mem) >> add_shift; // masks top 10 bits of addr
    page_dir[0].fourkb.present = 1;// marks as present
    page_dir[0].fourkb.su = 1; // marks as user

    // init 4 mb kernel page
    page_dir[1].fourmb.addr = (pte_size * num_pde) >> add_shift;// masks top 10 bits of addr
    page_dir[1].fourmb.present = 1;// marks as present
    page_dir[1].fourmb.rw = 1; // allows writing as well
    page_dir[1].fourmb.ps = 1; // sets page size


    // set page dir
    loadPageDirectory(page_dir);
    enablePaging();
}

/* switch_vram()
 * Description: saves displaying vram to page of old terminal, then copys the nondisplaying vram page
 *  of the new terminal into the displaying vram.
 * Inputs: idx - 1,2, or 3. The terminal # we want to map to vram
 */
uint32_t switch_vram(uint8_t oldIdx, uint8_t newIdx){
    // terminal's vram map
    uint32_t newIndex = (VIDEO >> add_shift) + newIdx; // index into page table of corresponding
    uint32_t oldIndex = (VIDEO >> add_shift) + oldIdx; // index into page table of corresponding

    // save vram to old terminal page
    memcpy(video_mem[oldIndex].addr << add_shift, VIDEO, FOURKB);
    // restore vram of new terminal page
    memcpy(VIDEO, video_mem[newIndex].addr << add_shift, FOURKB);

    // switch mapping
    // video_mem[newIndex].addr = (VIDEO >> add_shift);
    // video_mem[oldIndex].addr = (VIDEO >> add_shift) + oldIdx;
    return 0;
}

// void remap_VRAM(uint32_t physical_address){
//     //uint32_t pde = virtual_address / FOURMB;
//     video_mem[0].addr = physical_address >> add_shift;
//     video_mem[0].present = 1;

//     //flush the TLB here 
//     asm volatile("\
//     mov %cr3, %eax    ;\
//     mov %eax, %cr3    ;\
//     ");
// }

uint32_t allocate_4MB_page(uint32_t page_directory_idx, uint32_t pid){
    page_dir[page_directory_idx].fourmb.addr = (kernel_PDE >> 22) + pid;
    page_dir[page_directory_idx].fourmb.present = 1;// marks as present
    page_dir[page_directory_idx].fourmb.rw = 1; // allows writing as well
    page_dir[page_directory_idx].fourmb.ps = 1; // sets page size
    return 0;
}

/* allocate_4MB_page()
 * 
 * Inputs: vaddr - virtual address to allocate 4MB page
 * Return Value: SUCCESS=0, FAIL=-1;
 */
// uint32_t allocate_4MB_page(uint32_t vaddr, uint32_t physaddr){
//     if (vaddr % FOURMB != 0 || physaddr % FOURMB != 0) // if vaddr isn't divisiable by 4096 (4MB)
//         return -1;
//     if (page_dir[vaddr/FOURMB].fourmb.present) // if 4MB page already exists at vaddr
//         return -1;
//     page_dir[vaddr/FOURMB].fourmb.addr = physaddr >> add_shift;
//     page_dir[vaddr/FOURMB].fourmb.present = 1;
//     page_dir[vaddr/FOURMB].fourmb.rw = 1;
//     page_dir[vaddr/FOURMB].fourmb.ps = 1;

//     return 0;
// }   
