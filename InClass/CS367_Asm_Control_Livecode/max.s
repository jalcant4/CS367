.text # What follows goes in the .text section
.globl main #Make main global

main: # Main Label (Ignore main, we'll get back to functions later)
 	movq $80, %rdi	# X 
	movq $30, %rsi	# Y 
	callq max
	retq #For these constants, should return 8 
	# Note, Bash can only handle return codes <= 255

# Returns the bigger argument.
# X is in RDI, Y is in RSI
max: 
  cmpq %rsi, %rdi  # compare x vs. y
  jge else 
  movq %rsi, %rax  # result = y
  jmp end
else:
  movq %rdi, %rax  # result = x
end:
  retq             # return result (rax)

