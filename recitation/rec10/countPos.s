	.text
	.global countPos
	.type   countPos,@function

countPos:

	# Assume that RDI = arr (address), RSI = n (number of elements)
	# Goal: return the number of elements in arr with a positive value
	# ADD your code here


end:
	ret # Returns what is in RAX
	# END your code

	.size   countPos, .-countPos
