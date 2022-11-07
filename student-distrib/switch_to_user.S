.text
.globl user_switch
user_switch:
pushl $0x002b
movw $0x2b, %ax
mov %ax, %ds

pushl $0x83ffffc

pushfl
popl %eax
orl $0x200,%eax
pushl %eax

pushl $0x0023

pushl user_eip

iret