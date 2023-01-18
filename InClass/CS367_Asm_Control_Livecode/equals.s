.text # What follows goes in the .text section
.globl main #Make main global

main: # Main Label (Ignore main, we'll get back to functions later)
 	movq $2, %rdi	# X 
	movq $8, %rsi	# Y 
	callq equals
	retq #For these constants, should return 0
	# Note, Bash can only handle return codes <= 255

# if x == y, return 1, else return 0
equals: 			# x is in RDI, y is in RSI 




	ret 			# Returns what is in RAX 

