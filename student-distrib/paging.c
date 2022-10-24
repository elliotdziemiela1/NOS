#include "paging.h"

#define VIDEO       0xB8000
#define kernel_PDE 0x400000
#define pte_size 4096
#define add_shift 12
#define num_pde 1024
#define num_pte 1024

// create page directory (page_dir) and page table (video_mem)
page_directory_entry_t page_dir[num_pde] __attribute__((aligned(pte_size)));
page_table_entry_t video_mem[num_pte] __attribute__((aligned(pte_size)));

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
        video_mem[i].su = 0;
        video_mem[i].rw = 1;

        video_mem[i].pat = 0;
        video_mem[i].g = 0;

        video_mem[i].dirty = 0;
        video_mem[i].a = 0;

        video_mem[i].pcd = 0;
        video_mem[i].pwt = 0;
    }

    // set video memory
    uint32_t idx = VIDEO >> add_shift; // masks top 20 bits of addr
    video_mem[idx].present = 1; // marks as present



    // page directory setup
    for(i = 0; i < num_pde; i++){
        // init 4 kb
        page_dir[i].fourkb.addr = 0x0000;
        page_dir[i].fourkb.present = 0;
        page_dir[i].fourkb.su = 0;
        page_dir[i].fourkb.rw = 1;

        page_dir[i].fourkb.g = 0;

        page_dir[i].fourkb.ps = 0;
        page_dir[i].fourkb.a = 0;
        page_dir[i].fourkb.avl = 0;

        page_dir[i].fourkb.pcd = 0;
        page_dir[i].fourkb.pwt = 0;
    }

    // init 4 kb
    page_dir[0].fourkb.addr = (int)(video_mem) >> add_shift;// masks top 20 bits of addr
    page_dir[0].fourkb.present = 1;// marks as present
    page_dir[0].fourkb.su = 1; // marks as supervisor

    // init 4 mb
    page_dir[1].fourmb.addr = (pte_size * num_pde) >> add_shift;// masks top 20 bits of addr
    page_dir[1].fourmb.present = 1;// marks as present
    page_dir[1].fourmb.rw = 1; // allows writing as well
    page_dir[1].fourmb.ps = 1; // sets page size

    // set page dir
    loadPageDirectory(page_dir);
    enablePaging();
}
