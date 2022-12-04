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

    page_dir[3].fourmb.addr = 3*(pte_size * num_pde) >> add_shift;// masks top 10 bits of addr
    page_dir[3].fourmb.present = 1;// marks as present
    page_dir[3].fourmb.rw = 1; // allows writing as well
    page_dir[3].fourmb.ps = 1; // sets page size


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
    uint32_t newIndex = (VIDEO >> add_shift) + newIdx+1; // +1 due to 0 indexed value
    uint32_t oldIndex = (VIDEO >> add_shift) + oldIdx+1; // +1 due to 0 indexed value

    // save vram to old terminal page
    memcpy((void*)(video_mem[oldIndex].addr << add_shift), (void*)VIDEO, FOURKB);
    // restore vram of new terminal page
    memcpy((void*)VIDEO, (void*)(video_mem[newIndex].addr << add_shift), FOURKB);

    return 0;
}

uint32_t allocate_4MB_page(uint32_t page_directory_idx, uint32_t pid){
    page_dir[page_directory_idx].fourmb.addr = (kernel_PDE >> 22) + pid; // 22 is the offset into the pde
    page_dir[page_directory_idx].fourmb.present = 1;// marks as present
    page_dir[page_directory_idx].fourmb.rw = 1; // allows writing as well
    page_dir[page_directory_idx].fourmb.ps = 1; // sets page size
    return 0;
}

