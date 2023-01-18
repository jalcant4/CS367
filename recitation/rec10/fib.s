#jed alcantara

	.text
	.global fib
	.type   fib,@function

fib:
	# Assume that RDI = n
	# Goal: Return the nth Fibonacci Number.
	#       F_n = F_(n-1) + F_(n-2) when n > 1 else F_n = n.
	# ADD your code here

	# parameter 1 in %rdi = n
	# parameter 2 in %rsi
	# parameter 3 in %rdx
	# parameter 4 in %rcx

	#use only %rax, %rdi, %rsi, rdx, rcx r8 r9 r10 r11
	movq $0, %r8 #set register to 0
	movq $1, %r9 #set register to 1	
	leaq (%r8, %r9), %r10 #add two registers
	movq %r9, %r8
	movq %r10, %r9
	  
end:
	ret # Returns what is in RAX
	# END your code


	.size   fib, .-fib
