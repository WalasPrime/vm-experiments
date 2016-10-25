# A simple test script that counts to 10

    MOV R0 0
    MOV R10 1024   # Address
counter:
    ADD R0 1
    MOV R0 ACC
    SAVE R0 R10
    ADD R10 1
    MOV R10 ACC
    CMP R0 10
    JL @counter

    BREAK
