	lw 1 0 five
	lw 2 0 two
	jalr 4 1
	add 3 0 1
	beq 0 0 end
	add 3 0 2 # if jalr worked, then register 3 should end as a 2
end halt # if beq worked then it should halt on 6 instructions
	two	.fill 2
	five	.fill 5
