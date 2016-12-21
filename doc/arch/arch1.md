# Architecture 1
The most basic architecture for VMs to be implemented. Includes subprograms, integer operations and flow control. No Input-Output is defined (pure benchmarking of programs).

## Memory
Addressed with 32 bits. No segmentation is defined (the whole memory is a huge segment). Each address points at a 32 bit value. It is recommended to implement dynamic memory management in every VM (eg. divide the memory space into segments allocated at runtime if accessed). Loaded programs will be put at the begining of the memory space. The stack is also allocated in this memory (defined with an offset), the stack grows incrementaly (after using `PUSH` the stack pointer will increase). Arithmetics are defined only for *unsigned 32 bit* values.

## CPU
Register name | Size | Notes
---- | ---- | ----
PC | 32 bit | Instruction counter (current instruction memory offset), *read-only*
FLAGS | 8 bit | Flag register (xxxx xLZC)
ACC | 32 bit | Result register
DS | 32 bit | Data offset
SS | 32 bit | Stack offset
SP | 32 bit | Stack pointer (current stack value)
R0 | 32 bit
... | ... | 12 general R* registers in total
R11 | 32 bit

Flag register identifier | Meaning
---- | ----
x | Bit without meaning
C | Carry flag - set if the result of ADD/SUB result in overflow
L | Less flag - set if the result is negative, or CMP result "A < B"
Z | Zero flag - set if the result of the last operation is zero (or A = B in case of CMP)


## Instructions
Name | Instruction usage | Definition | Affected flags/Notes
---- | ---- | ---- | ----
MOV | MOV REG1 REG2/VAL | REG1 = REG2/VAL | Z
ADD | ADD REG1 REG2/VAL | ACC = REG1 + REG2/VAL | CZ
ADC | ADC REG1 REG2/VAL | ACC = REG1 + REG2/VAL + (FLAGS[C] ? 1 : 0) | CZ
SUB | SUB REG1 REG2/VAL | ACC = REG1 - REG2/VAL | CZL
SBC | SBC REG1 REG2/VAL | ACC = REG1 - REG2/VAL - (FLAGS[C] ? 1 : 0) | CZL
CLF | | FLAGS=0 | Clear flags
LOAD | LOAD REG1 REG2/VAL | REG1 = [DS:REG2/VAL] | Load a 32 bit value from the memory, addressed with DS + VAL, into any register
SAVE | SAVE REG1 REG2/VAL | [DS:REG2/VAL] = REG1 | Save a 32 bit value from any register to address DS + VAL
CMP | CMP REG1 REG2/VAL | Compare two register values, result is saved within flags | LZ
JMP | JMP REG/VAL | | Jump to the target instruction number
JE | JE REG/VAL | | Jump to the target instruction number (if Z flag is up)
JNE | JNE REG/VAL | | Jump to the target instruction number (if Z flag is down)
JL | JL REG/VAL | | Jump to the target instruction number (if L flag is up)
JLE | JLE REG/VAL | | Jump to the target instruction number (if L or Z flag is up)
JG | JG REG/VAL | | Jump to the target instruction number (if L and Z flags are down)
JGE | JGE REG/VAL | | Jump to the target instruction number (if L flag is down)
PUSH | PUSH REG | [SS:SP] = REG; SP++ | Push a register value to the stack
POP | POP REG | REG = [SS:SP]; SP-- | Pop a value into a register from the stack
CALL | CALL REG/VAL | PUSH PC; JMP VAL | Push the current instruction number onto the stack then jump to the target instruction number (procedures and functions)
RET | RET | POP PC; PC++ | Pop an address from the stack then jump *after* it (end of procedures and functions)
BREAK | BREAK | | Forcefully stop VM execution (breakpoint)