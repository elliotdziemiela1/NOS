#define S_USER_DS 0x002b
#define S_USER_CS 0x0023
#define S_ESP 0x83ffffc

.text
.globl user_switch
user_switch:
pushl $S_USER_DS
movw $S_USER_DS, %ax
mov %ax, %ds
pushl $S_ESP
pushfl
popl %eax
orl $0x200,%eax # bitmask for flag 
pushl %eax
pushl $S_USER_CS
pushl user_eip
iret
