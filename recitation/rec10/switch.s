	.file	"switch.c"
	.text
	.globl	do_switch
	.type	do_switch, @function
do_switch:
.LFB8:
	.cfi_startproc
	cmpq	$6, %rdi
	ja	.L2
	jmp	*.L4(,%rdi,8)
	.section	.rodata
	.align 8
	.align 4
.L4:
	.quad	.L2
	.quad	.L7
	.quad	.L8
	.quad	.L2
	.quad	.L5
	.quad	.L3
	.quad	.L3
	.text
.L7:
	movl	$16, %eax
	jmp	.L6
.L5:
	movl	$16384, %eax
	jmp	.L6
.L3:
	movl	$96, %eax
	jmp	.L6
.L2:
	movl	$0, %eax
	jmp	.L6
.L8:
	movl	$512, %eax
.L6:
	imulq	%rax, %rax
	ret
	.cfi_endproc
.LFE8:
	.size	do_switch, .-do_switch
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Switching on %ld\n"
.LC1:
	.string	"Returned %ld\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB9:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	$0, %edi
	call	time
	movl	%eax, %edi
	call	srand
	call	rand
	movl	%eax, %ecx
	movl	$-1840700269, %edx
	imull	%edx
	leal	(%rdx,%rcx), %edi
	sarl	$2, %edi
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edi
	leal	0(,%rdi,8), %eax
	subl	%edi, %eax
	subl	%eax, %ecx
	movslq	%ecx, %rbx
	movq	%rbx, %rsi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	movq	%rbx, %rdi
	call	do_switch
	movq	%rax, %rsi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	printf
	movl	$0, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE9:
	.size	main, .-main
	.ident	"GCC: (GNU) 8.5.0 20210514 (Red Hat 8.5.0-4)"
	.section	.note.GNU-stack,"",@progbits
