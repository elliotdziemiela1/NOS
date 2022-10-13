#include "synch.h"

#define GET_FLAGS(X) asm volatile ("pushfl;\
                                    popl %%eax;       \
                                    movl %%eax, %0;"  \
                                    :"=m" (X)         \
                                    ); 
#define SET_FLAGS(X) asm volatile ("pushfl;\
                                    popl %%eax;       \
                                    movl %%eax, %0;"  \
                                    :"=m" (X)         \
                                    ); 
#define CLI() asm volatile ("cli;\
                    ); 
#define STI() asm volatile ("sti;\
                    ); 


spin_lock new_lock(){
    spin_lock ret;
    ret.locked = 1;
    return ret;
}
void spin_lock_irsave(spin_lock * lock, unsigned long * flags){
    CLI();
    GET_FLAGS(flags);
    while (1){
        if (!lock.locked){
            lock.locked=1;
            return;
        }
    }
}
void spin_unlock_irrestore(spin_lock * lock, unsigned long * flags){
    STI();
    SET_FLAGS(flags);
    lock.locked = 0;
}