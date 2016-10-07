# Comment

my_func: # Comment
	add r0 r1#Comment
	mov r0 acc
	jmp @f_check

_start:
	mov r0 5
	mov r1 10
	jmp @my_func
	
f_check:
	