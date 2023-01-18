	.text
	.global power
	.type   power,@function

power:

	# Assume that RDI = x and RSI = n
        # Goal: Compute x to the power of n
	# ADD your code here

end:
	ret # Returns what is in RAX
	# END your code
	
	.size   power, .-power
