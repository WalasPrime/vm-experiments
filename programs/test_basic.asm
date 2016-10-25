# Comment
	mov r0 1
	mov r1 2
	mov r2 3
	mov r3 4
jmp @_start

my_func: # Comment
	add r0 r1#Comment
	mov r0 acc
	jmp @f_check

_start:
	mov r0 5
	mov r1 10
	jmp @my_func
	
f_check:
	break