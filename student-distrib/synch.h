/* synch.h - api for locks/semaphores */

typedef struct spin_lock{
    int locked;
} spin_lock;

spin_lock new_lock();
void spin_lock_irsave(spin_lock & lock, unsigned long & flags);
void spin_unlock_irrestore(spin_lock & lock, unsigned long & flags);