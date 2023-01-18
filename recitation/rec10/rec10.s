	.file	"rec10.c"
	.text
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"error - wrong number of arguments.\n\n"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC1:
	.string	"\tusage: %s %s"
.LC2:
	.string	" arg%d"
.LC3:
	.string	"\n\n"
	.text
	.type	check_func_params, @function
check_func_params:
.LFB8:
	.cfi_startproc
	leal	2(%rcx), %eax
	cmpl	%edx, %eax
	jne	.L9
	movl	$1, %eax
	ret
.L9:
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movl	%ecx, %ebx
	movq	%rsi, %r12
	movq	%rdi, %rbp
	movq	stderr(%rip), %rcx
	movl	$36, %edx
	movl	$1, %esi
	movl	$.LC0, %edi
	call	fwrite
	movq	%r12, %rcx
	movq	%rbp, %rdx
	movl	$.LC1, %esi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf
	movl	$1, %ebp
	jmp	.L3
.L4:
	movl	%ebp, %edx
	movl	$.LC2, %esi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf
	addl	$1, %ebp
.L3:
	cmpl	%ebx, %ebp
	jle	.L4
	movq	stderr(%rip), %rcx
	movl	$2, %edx
	movl	$1, %esi
	movl	$.LC3, %edi
	call	fwrite
	movl	$1, %edi
	call	exit
	.cfi_endproc
.LFE8:
	.size	check_func_params, .-check_func_params
	.section	.rodata.str1.8
	.align 8
.LC4:
	.string	"error - not enough arguments.\n\n\tusage: %s funcname arg1 [arg2 arg3 ...]\n\n"
	.align 8
.LC5:
	.string	"error - memory allocation failed.\n\n"
	.section	.rodata.str1.1
.LC6:
	.string	"checkeq"
.LC7:
	.string	"%ld\n"
.LC8:
	.string	"power"
.LC9:
	.string	"countPos"
.LC10:
	.string	"fib"
.LC11:
	.string	"collatzLength"
	.section	.rodata.str1.8
	.align 8
.LC12:
	.string	"error - unrecognized command '%s'.\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB9:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	%rsi, %r13
	cmpl	$2, %edi
	jle	.L25
	movl	%edi, %ebp
	movslq	%edi, %rbx
	movl	$8, %esi
	movq	%rbx, %rdi
	call	calloc
	movq	%rax, %r12
	testq	%rax, %rax
	je	.L26
	leaq	0(,%rbx,8), %rdx
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset
	movl	$2, %ebx
.L14:
	cmpl	%ebp, %ebx
	jge	.L27
	movslq	%ebx, %rax
	salq	$3, %rax
	leaq	(%r12,%rax), %r14
	movl	$0, %edx
	movl	$0, %esi
	movq	0(%r13,%rax), %rdi
	call	strtol
	movq	%rax, (%r14)
	addl	$1, %ebx
	jmp	.L14
.L25:
	movq	(%rsi), %rdx
	movl	$.LC4, %esi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf
	movl	$1, %eax
	jmp	.L10
.L26:
	movq	stderr(%rip), %rcx
	movl	$35, %edx
	movl	$1, %esi
	movl	$.LC5, %edi
	call	fwrite
	movl	$1, %eax
	jmp	.L10
.L27:
	movq	0(%r13), %r14
	movq	8(%r13), %r13
	movq	%r13, %rsi
	movl	$.LC6, %edi
	call	strcmp
	testl	%eax, %eax
	je	.L28
	movq	%r13, %rsi
	movl	$.LC8, %edi
	call	strcmp
	testl	%eax, %eax
	je	.L29
	movq	%r13, %rsi
	movl	$.LC9, %edi
	call	strcmp
	movl	%eax, %ebx
	testl	%eax, %eax
	je	.L30
	movq	%r13, %rsi
	movl	$.LC10, %edi
	call	strcmp
	testl	%eax, %eax
	je	.L31
	movq	%r13, %rsi
	movl	$.LC11, %edi
	call	strcmp
	testl	%eax, %eax
	jne	.L23
	movl	$1, %ecx
	movl	%ebp, %edx
	movq	%r13, %rsi
	movq	%r14, %rdi
	call	check_func_params
	movq	16(%r12), %rdi
	call	collatzLength
	movq	%rax, %rsi
	movl	$.LC7, %edi
	movl	$0, %eax
	call	printf
	jmp	.L17
.L28:
	movl	$2, %ecx
	movl	%ebp, %edx
	movq	%r13, %rsi
	movq	%r14, %rdi
	call	check_func_params
	movq	16(%r12), %rdi
	movq	24(%r12), %rsi
	call	checkeq
	movq	%rax, %rsi
	movl	$.LC7, %edi
	movl	$0, %eax
	call	printf
.L17:
	movq	%r12, %rdi
	call	free
	movl	$0, %eax
.L10:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L29:
	.cfi_restore_state
	movl	$2, %ecx
	movl	%ebp, %edx
	movq	%r13, %rsi
	movq	%r14, %rdi
	call	check_func_params
	movq	16(%r12), %rdi
	movq	24(%r12), %rsi
	call	power
	movq	%rax, %rsi
	movl	$.LC7, %edi
	movl	$0, %eax
	call	printf
	jmp	.L17
.L30:
	subl	$2, %ebp
	movslq	%ebp, %r14
	movl	$8, %esi
	movq	%r14, %rdi
	call	calloc
	movq	%rax, %r13
	testq	%rax, %rax
	jne	.L20
	movq	stderr(%rip), %rcx
	movl	$35, %edx
	movl	$1, %esi
	movl	$.LC5, %edi
	call	fwrite
	movl	$1, %eax
	jmp	.L10
.L21:
	movslq	%ebx, %rax
	movq	16(%r12,%rax,8), %rdx
	movq	%rdx, 0(%r13,%rax,8)
	addl	$1, %ebx
.L20:
	cmpl	%ebx, %ebp
	jg	.L21
	movq	%r14, %rsi
	movq	%r13, %rdi
	call	countPos
	movq	%rax, %rsi
	movl	$.LC7, %edi
	movl	$0, %eax
	call	printf
	movq	%r13, %rdi
	call	free
	jmp	.L17
.L31:
	movl	$1, %ecx
	movl	%ebp, %edx
	movq	%r13, %rsi
	movq	%r14, %rdi
	call	check_func_params
	movq	16(%r12), %rdi
	call	fib
	movq	%rax, %rsi
	movl	$.LC7, %edi
	movl	$0, %eax
	call	printf
	jmp	.L17
.L23:
	movq	%r13, %rdx
	movl	$.LC12, %esi
	movq	stderr(%rip), %rdi
	movl	$0, %eax
	call	fprintf
	movq	%r12, %rdi
	call	free
	movl	$2, %eax
	jmp	.L10
	.cfi_endproc
.LFE9:
	.size	main, .-main
	.ident	"GCC: (GNU) 8.5.0 20210514 (Red Hat 8.5.0-4)"
	.section	.note.GNU-stack,"",@progbits
