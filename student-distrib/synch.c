#include "synch.h"
#include "lib.h"


spin_lock new_lock(){
    spin_lock ret;
    ret.locked = 0;
    return ret;
}
void spin_lock_irsave(spin_lock * lock, unsigned long * flags){
    cli_and_save(flags);
    while (1){
        if (!lock->locked){
            lock->locked=1;
            return;
        }
    }
}
void spin_unlock_irrestore(spin_lock * lock, unsigned long flags){
    sti();
    restore_flags(flags);
    lock->locked = 0;
}
