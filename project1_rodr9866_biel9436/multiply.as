	lw 1 0 mcand
	lw 2 0 mplier
	lw 3 0 one #nand bit comparator increments
	lw 6 0 zero	#accumulator must be less than 15
	lw 7 0 zero #product result
start	lw 4 0 min
	nand 5 2 4	 #isolates significant bit
	nand 5 5 4		#isolates significant bit
	beq 0 5 if	 #checks if bit is 0, if yes, shifts mcand but does not add to product
	add 7 7 1 	#adds mcand to product
if	add 1 1 1 	#mcand <<1
	add 3 3 3	#comparator <<1
	lw 4 0 one	#increments counter
	add 6 6 4 	#increments counter
	lw 4 0 fifteen #loads 15 to be checked
	beq 6 4 end 	#checks if run 15 times
	beq 0 0 start	#loops back to start
mcand	 .fill 29562
mplier	 .fill 11834
one	 .fill 1
min	 .fill -1
zero	 .fill 0
fifteen	.fill 15
