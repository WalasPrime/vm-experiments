# Arithmetics test

	MOV SS 2048
	MOV R10 1024
	MOV R5 1
	jmp @_start

_start:
	MOV R0 5
	ADD R0 5
	CMP ACC 10
	JNE @error
	CALL @score

	ADD R0 R0
	CMP ACC 10
	JNE @error
	CALL @score

	MOV R1 4294967295 # 2^32-1
	MOV R0 R1
	ADD R0 1
	CMP ACC 0
	JNE @error
	CALL @score

	MOV R1 0
	SUB R1 1
	CMP ACC 4294967295
	JNE @error
	CALL @score

	jmp @end

score:
	SAVE R5 R10
	ADD R5 1
	MOV R5 ACC
	ADD R10 1
	MOV R10 ACC
	RET

end:
	break

error:
	break