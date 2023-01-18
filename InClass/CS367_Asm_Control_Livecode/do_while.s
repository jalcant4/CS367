.text # What follows goes in the .text section
.globl main #Make main global

main: # Main Label (Ignore main, we'll get back to functions later)
 	movq $2, %rdi	# X 
	movq $8, %rsi	# Y 
	callq incr
	retq #For these constants, should return 8 
	# Note, Bash can only handle return codes <= 255
# for(i = 0; i < y; i++) {
#   x++;
# }
# return x;
incr: 			# x is in RDI, y is in RSI 
  movq $0, %rcx  # i = 0
  jmp compare
top:
  incq %rdi # x++
  incq %rcx # i++
compare:
  cmpq %rsi, %rcx  # compare i vs. Y
  jl top
  movq %rdi, %rax  # res = X
	retq      # return res
