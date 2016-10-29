# A test trying out cmp and jmp instructions

	MOV R10 1024    # Start saving here
	MOV R11 1       # Score
_start:
	SAVE R11 R10
	jmp @_step1
	break
_step1:
	ADD R11 1       # Increase score
	SAVE ACC R10
	MOV R11 ACC

	MOV R0 10
	CMP R0 15
	JL @_step2
	break
_step2:
	ADD R11 1       # Increase score
	SAVE ACC R10
	MOV R11 ACC

	MOV R1 5
	CMP R0 R1
	JG @_step3
	break
_step3:
	ADD R11 1       # Increase score
	SAVE ACC R10
	MOV R11 ACC

	MOV R1 10
	CMP R1 10
	JE @_step4
	break
_step4:
	ADD R11 1       # Increase score
	SAVE ACC R10
	MOV R11 ACC

	MOV R1 20
	CMP R1 25
	JNE @_step5
	break
_step5:
	ADD R11 1       # Increase score
	SAVE ACC R10
	MOV R11 ACC
					# Add more tests?
	break