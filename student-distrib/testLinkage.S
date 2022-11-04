.GLOBL mytest                   ;\
mytest:   PUSHL	%EBX          ;\
	MOVL	$4,%EAX  ;\
	MOVL	$0,%EBX  ;\
	MOVL	$0,%ECX ;\
	MOVL	$0,%EDX ;\
	INT	$0x80         ;\
	POPL	%EBX          ;\
	RET