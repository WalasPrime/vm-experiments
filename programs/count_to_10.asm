# A simple test script that counts to 10

    CMOV R0 0
    CMOV R10 1024   # Address
counter:
    ADD R0 1
    MOV R0 ACC
    SAVE R0 R10
    CMP R0 10
    JL @counter

    BREAK
