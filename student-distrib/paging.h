#pragma once

#include "lib.h"

void init_paging();

// page table struct
typedef struct __attribute__ ((packed)) {
    uint32_t present    : 1; // present
    uint32_t rw         : 1; // read/write
    uint32_t su         : 1; // supervisor/user

    uint32_t pwt        : 1; // page write through
    uint32_t pcd        : 1; // page cache disable
    uint32_t a          : 1; // accessed
    uint32_t dirty      : 1; // dirty

    uint32_t pat        : 1; // page attribute table
    uint32_t g          : 1; // global
    uint32_t avl        : 3; // available
    uint32_t addr       :20; // page address
} page_table_entry_t;


// page dir 4 mb struct
typedef union four_mb {
    struct{
        uint32_t present    : 1; // present
        uint32_t rw         : 1; // read/write
        uint32_t su         : 1; // supervisor/user

        uint32_t pwt        : 1; // page write through
        uint32_t pcd        : 1; // page cache disable
        uint32_t a          : 1; // accessed
        uint32_t dirty      : 1; // dirty
        uint32_t ps         : 1; // page size

        uint32_t g          : 1; // global
        uint32_t avl        : 3; // available
        uint32_t addr       :20; // address
    } __attribute__ ((packed));
} four_mb;


// page dir 4 kb struct
typedef union four_kb {
    struct{
        uint32_t present    : 1; // present
        uint32_t rw         : 1; // read/write
        uint32_t su         : 1; // supervisor/user

        uint32_t pwt        : 1; // page write through
        uint32_t pcd        : 1; // page cache disable
        uint32_t a          : 1; // accessed
        uint32_t dirty      : 1; // dirty
        uint32_t ps         : 1; // page size

        uint32_t g          : 1; // global
        uint32_t avl        : 3; // available
        uint32_t addr       :20; // address
    } __attribute__ ((packed));
} four_kb;


// page dir struct
typedef union page_directory_entry_t {
    four_kb fourkb;
    four_mb fourmb;
} page_directory_entry_t;


extern void enablePaging();
extern void loadPageDirectory (page_directory_entry_t *page_dir);
