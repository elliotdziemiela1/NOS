# .text
# .globl switch_to_user
# switch_to_user:
# andl $0x00FF, %ebx
# movw %bx, %ds
# pushl %ebx
# pushl %edx
# pushfl
# popl %edx
# orl $0x2000, %edx
