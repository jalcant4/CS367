	.text
	.global checkeq
	.type   checkeq,@function


checkeq:
	# Assume that RDI = a and RSI = b
	# Goal: return 1 if a == b, or 0 otherwise
	# YOUR CODE HERE


	ret # returns what is in RAX
	# END YOUR CODE
	
	.size   checkeq, .-checkeq
