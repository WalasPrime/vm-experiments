# Stack tests

    MOV SS 2048 # Stack out of dump
    MOV R10 1024 # Dump here 
_start:
    MOV R0 0

_push_some:
    ADD R0 1
    MOV R0 ACC

    PUSH R0

    SAVE R0 R10
    ADD R10 1
    MOV R10 ACC

    CMP R0 10
    JL @_push_some

_pop_some:
    POP R1

    CMP R1 R0
    JNE @_error

    SAVE R1 R10
    ADD R10 1
    MOV R10 ACC

    SUB R0 1
    MOV R0 ACC

    CMP R0 0
    JE @_end
    JMP @_pop_some

_error:
    fail

_end:
    BREAK