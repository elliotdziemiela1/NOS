#include "paging.h"

#define VIDEO       0xB8000
#define kernel_PDE 0x400000
#define pte_size 4096
#define add_shift 12
#define num_pde 1024
#define num_pte 1024
#define FOURMB  4096*1024

// notes:
// - first 10 bits of virtual address are index into page_dir array
// - next 10 bits are index into PDEs page_table array
// - next 12 bits are offset into page selected from PTE

static int numUsedPDES = 0; // the number of used PDES. Also can refer to
// the index to insert the next PDE/4MB page.

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
    page_dir[0].fourkb.addr = (int)(video_mem) >> add_shift;// masks top 10 bits of addr
    page_dir[0].fourkb.present = 1;// marks as present
    page_dir[0].fourkb.su = 1; // marks as supervisor

    // init 4 mb kernel page
    page_dir[1].fourmb.addr = (pte_size * num_pde) >> add_shift;// masks top 10 bits of addr
    page_dir[1].fourmb.present = 1;// marks as present
    page_dir[1].fourmb.rw = 1; // allows writing as well
    page_dir[1].fourmb.ps = 1; // sets page size

    // set page dir
    loadPageDirectory(page_dir);
    enablePaging();
}

/* allocate_4MB_page()
 * 
 * Inputs: vaddr - virtual address to allocate 4MB page
 * Return Value: SUCCESS=0, FAIL=-1;
 */
uint32_t allocate_4MB_page(uint32_t vaddr, uint32_t physaddr){
    if (vaddr % FOURMB != 0 || physaddr % FOURMB != 0) // if vaddr isn't divisiable by 4096 (4MB)
        return -1;
    if (page_dir[vaddr/FOURMB].fourmb.present) // if 4MB page already exists at vaddr
        return -1;
    page_dir[vaddr/FOURMB].fourmb.addr = physaddr >> add_shift;
    page_dir[vaddr/FOURMB].fourmb.present = 1;
    page_dir[vaddr/FOURMB].fourmb.rw = 1;
    page_dir[vaddr/FOURMB].fourmb.ps = 1;

    return 0;
}   
