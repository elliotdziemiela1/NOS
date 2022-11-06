.GLOBL call_open                   ;\
call_open:   PUSHL	%EBX          ;\
	MOVL	$5,%EAX  ;\
	MOVL	$0,%EBX  ;\
	MOVL	$0,%ECX ;\
	MOVL	$0,%EDX ;\
	INT	$0x80         ;\
	POPL	%EBX          ;\
	RET

.GLOBL call_write                ;\
call_write:   PUSHL	%EBX          ;\
	MOVL	$4,%EAX  ;\
	MOVL	$0,%EBX  ;\
	MOVL	$0,%ECX ;\
	MOVL	$0,%EDX ;\
	INT	$0x80         ;\
	POPL	%EBX          ;\
	RET

.GLOBL my_do_call                ;\
my_do_call:   PUSHL	%EBX          ;\
	MOVL	8(%ESP),%EAX  ;\
	MOVL	$stringi,%EBX  ;\
	MOVL	$0,%ECX ;\
	MOVL	$0,%EDX ;\
	INT	$0x80         ;\
	POPL	%EBX          ;\
	RET

	stringi:
	.string "rtc"