	.file	"main.cc"
	.text
	.section	.rodata
.LC0:
	.string	"%d"
.LC1:
	.string	"Your age %d\n"
.LC3:
	.string	"pi = %f\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	-12(%rbp), %rax
	leaq	.LC0(%rip), %rdx
	movq	%rax, %rsi
	movq	%rdx, %rdi
	movl	$0, %eax
	call	__isoc23_scanf@PLT
	movl	-12(%rbp), %eax
	movl	$5, %edx
	cmpl	%edx, %eax
	cmovl	%edx, %eax
	leaq	.LC1(%rip), %rdx
	movl	%eax, %esi
	movq	%rdx, %rdi
	movl	$0, %eax
	call	_Z6printfPKcz@PLT
	movq	.LC2(%rip), %rax
	leaq	.LC3(%rip), %rdx
	movq	%rax, %xmm0
	movq	%rdx, %rdi
	movl	$1, %eax
	call	_Z6printfPKcz@PLT
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L3
	call	__stack_chk_fail@PLT
.L3:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC2:
	.long	-1065151889
	.long	1074340298
	.ident	"GCC: (GNU) 15.2.1 20250813"
	.section	.note.GNU-stack,"",@progbits
