# Comment

my_func: # Comment
	add r0 r1#Comment
	mov r0 acc
	jmp @f_check

_start:
	cmov r0 5
	cmov r1 10
	jmp @my_func
	
f_check:
	cmov r0 0