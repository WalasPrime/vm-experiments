# Procedures test

    MOV SS 2048 # Stack out of dump
	MOV R10 1024 # Dump here
	jmp @_start
count_down:
	# Argument in R5
	# If > 1 then push R5, call recursively, pop R5
	# save R5 in R10, increase R10
	CMP R5 1
	JE @count_down_ok

	PUSH R5
	SUB R5 1
	MOV R5 ACC
	CALL @count_down
	POP R5

	count_down_ok:
	SAVE R5 R10
	ADD R10 1
	MOV R10 ACC
	RET

_start:
	MOV R5 10
	call @count_down
	break
