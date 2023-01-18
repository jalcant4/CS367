.text # What follows goes in the .text section
.globl main #Make main global

main: # Main Label (Ignore main, we'll get back to functions later)
 	movq $12, %rdi	# X 
	movq $8, %rsi	# Y 
	callq max
	retq #For these constants, should return 8 
	# Note, Bash can only handle return codes <= 255

# Returns the bigger argument.
max:	# x is in RDI, y is in RSI 



	retq 			# Returns what is in RAX 
