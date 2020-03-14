	nand 1 0 0
	lw 2 0 ten #reg2 = 10
	sw 2 0 11	#mem[11] = 10
	lw 4 0 11 #reg4 = mem[11]
	halt #reg4 should end as the same value as reg2
ten .fill 10
