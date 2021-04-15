	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 10
	.globl	_shuffle
	.align	4, 0x90
_shuffle:                               ## @shuffle
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp0:
	.cfi_def_cfa_offset 16
Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp2:
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r12
	pushq	%rbx
Ltmp3:
	.cfi_offset %rbx, -48
Ltmp4:
	.cfi_offset %r12, -40
Ltmp5:
	.cfi_offset %r14, -32
Ltmp6:
	.cfi_offset %r15, -24
	movl	%esi, %r12d
	movq	%rdi, %r14
	cmpl	$2, %r12d
	jl	LBB0_3
## BB#1:                                ## %.lr.ph
	leal	-1(%r12), %eax
	movslq	%eax, %r15
	xorl	%ebx, %ebx
	.align	4, 0x90
LBB0_2:                                 ## =>This Inner Loop Header: Depth=1
	callq	_rand
	movl	%eax, %ecx
	movl	$2147483647, %eax       ## imm = 0x7FFFFFFF
	xorl	%edx, %edx
	idivl	%r12d
	movl	%eax, %esi
	incl	%esi
	movl	%ecx, %eax
	cltd
	idivl	%esi
	addl	%ebx, %eax
	cltq
	movl	(%r14,%rax,4), %ecx
	movl	(%r14,%rbx,4), %edx
	movl	%edx, (%r14,%rax,4)
	movl	%ecx, (%r14,%rbx,4)
	incq	%rbx
	decl	%r12d
	cmpq	%r15, %rbx
	jl	LBB0_2
LBB0_3:                                 ## %.loopexit
	popq	%rbx
	popq	%r12
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc


.subsections_via_symbols
