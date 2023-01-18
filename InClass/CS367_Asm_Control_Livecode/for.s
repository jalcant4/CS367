.text # What follows goes in the .text section
.globl main #Make main global

main: # Main Label (Ignore main, we'll get back to functions later)
 	movq $2, %rdi	# X 
	movq $8, %rsi	# Y 
	callq incr
	retq #For these constants, should return 8 
	# Note, Bash can only handle return codes <= 255

# do {
#  x++;
# } while(x < y);
# return x;
incr: 			# x is in RDI, y is in RSI 
movq %rdi, %rdx   # i = X
jmp compare
top:
  incq %rdi   # x++
  incq %rdx   # i++
compare:
  cmpq %rsi, %rdx   # compare i vs. y
  jl top

  movq %rdi, %rax   # res = x;
	retq      # return res
