.data
    PARAM_MIN = 1
    PARAM_MAX = 10

.text
.globl sys_call_linkage

# extern halt #1
# extern execute #2
# extern read #3
# extern write #4
# extern open #5

# this is the system call linkage function for system calls
sys_call_linkage:
    pushl %ebp
    pushl %esi
    pushl %edi
    pushfl
    #pushing arguments to the stack
    pushl %edx # 1st Argument
    pushl %ecx # 2nd Argument
    pushl %ebx # 3rd Argument

    #parameter validation
    cmpl $PARAM_MIN, %eax
    jl invalid
    
    cmpl $PARAM_MAX, %eax
    jg invalid

    #not sure if interrupts need to be cleared here
    #still need to include assembly linkage
    
    call *jump_table(, %eax, 4)

    #teardown stack
    popl %ebx
    popl %ecx
    popl %edx
    popfl
    popl %edi
    popl %esi
    popl %ebp
    iret

invalid:
    #teardown stack
    popl %ebx
    popl %ecx
    popl %edx
    popfl
    popl %edi
    popl %esi
    popl %ebp
    #return -1 
    movl $-1, %eax
    iret


jump_table:
    .long 0x0
    .long halt #1
    .long execute #2
    .long read #3
    .long write #4
    .long open #5
    .long close #6
    # .long getargs #7
    # .long vidmap #8
    # .long set_handler #9
    # .long sigreturn #10
