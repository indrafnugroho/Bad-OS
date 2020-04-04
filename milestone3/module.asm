	.file	"mathmodule.c"
	.text
	.globl	mod
	.type	mod, @function
mod:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	jmp	.L2
.L3:
	movl	-8(%rbp), %eax
	subl	%eax, -4(%rbp)
.L2:
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	jge	.L3
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	mod, .-mod
	.globl	div
	.type	div, @function
div:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L6
.L7:
	addl	$1, -4(%rbp)
.L6:
	movl	-4(%rbp), %eax
	imull	-24(%rbp), %eax
	cmpl	%eax, -20(%rbp)
	jge	.L7
	movl	-4(%rbp), %eax
	subl	$1, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	div, .-div
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
