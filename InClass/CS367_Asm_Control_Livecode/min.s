.text # What follows goes in the .text section
.globl main #Make main global

main: # Main Label (Ignore main, we'll get back to functions later)
 	movq $500, %rdi	# X 
	movq $30, %rsi	# Y 
	callq min
	retq #For these constants, should return 8 
	# Note, Bash can only handle return codes <= 255

# Returns the smaller argument.
min: 




  retq             # return rax

