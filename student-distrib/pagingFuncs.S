
# place address of directory in cr3
.text
.globl loadPageDirectory

loadPageDirectory:
push %ebp
mov %esp, %ebp
mov 8(%esp), %eax
mov %eax, %cr3
mov %ebp, %esp
pop %ebp
ret

.text
.globl enablePaging
enablePaging:
push %ebp
mov %esp, %ebp
mov %cr4, %eax
or $0x00000010, %eax
mov %eax, %cr4
mov %cr0, %eax
or $0x80000001, %eax
mov %eax, %cr0
mov %ebp, %esp
pop %ebp
ret

.text
.globl flush_tlb
flush_tlb:
mov %cr3, %eax
mov %eax, %cr3