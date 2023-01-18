	.text
	.global call_dot4
	.type   call_dot4,@function
call_dot4:
	# YOUR CODE HERE
	movq $1, %rdi
	movq $2 %rsi
	movq $3, %rdx
	movq $4, %rcx
	movq $4, %r8
	movq $3, %r9
	pushq $2
	pushq $1
	call dot4
dot4:
	add $16 %rsp
	imulq %r8, %rdi
	imulq %r9, %rsi
	imulq 8(%rsp), %rdx 	#add 8 to rsp
	imulq 16(%rsp), %rcx	#add 16 to rsp
end:
	ret
	# END YOUR CODE	
	.size   call_dot4, .-call_dot4
