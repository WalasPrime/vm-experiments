# Virtual Machine Experiments ![Travis Buildstatus](https://travis-ci.org/WalasPrime/vm-experiments.svg)
Very simple Virtual Machines (custom architecture) written in many languages for benchmarking purpose. A small project made to help understand the practical side of simulating a real logical computer architecture (not a physical one, we don't simulate delays on the transistors).

## Building
This project uses SCons 2.5.0 as it's buildsystem (which requires Python 2.7). After cloning this repo just execute the `scons` command to build all the executables.

## Currently implemented machines
Machine name | Description | Status
---- | ---- | ----
C | Pure C machine | On-hold (*had some issues with the old C syntax*)
Cpp | Mixed C/C++ machine | Work-in-progress

## Architecture
The following description is subject to change over time.

### Memory
Addressed with 32 bits. No segmentation is defined (the whole memory is a huge segment). Each address points at a 32 bit value. It is recommended to implement dynamic memory management in every VM (eg. divide the memory space into segments allocated at runtime if accessed). Loaded programs will be put at the begining of the memory space. The stack is also allocated in this memory (defined with an offset), the stack grows incrementaly (after using `PUSH` the stack pointer will increase).

### CPU
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
L | Less flag - set if the result of a CMP is "A < B"
Z | Zero flag - set if the result of the last operation is zero (or A = B in case of CMP)


### Assembly markdown
All VM machines must be able to load, compile and execute files containing assembly code for the defined CPU architecture. Opcode format can be defined as per-machine.
The following Assembly features are defined:

Feature name | Definition | Code example | Notes
---- | ---- | ---- | ----
Instructions | [WHITESPACES]* [INSTRUCTION_NAME] [INSTRUCTION_ARGUMENTS]* | ADD R0 R1 | A line of code may define an instruction to execute by the CPU. It may be preceeded by any number of whitespace characters (spaces, tabs) and followed by arguments, which may be etiquettes. Instructions are not case-sensitive.
Etiquette | [ETIQUETTE_NAME]: | my_function: | An etiquette allows the program to jump to the next instruction in the code (or to the next instruction after the one preceeding the etiquette).
Etiquette reference | @[ETIQUETTE_NAME] | @my_function | If a reference is used, it should be replaced with the real value (address number). Etiquettes do not need to be defined before are used (but have to be defined later - multipass parsing is required).
Comment | # [GENERIC_CONTENT] | # Some comment here | A comment sign `#` means the rest of the current line is to be ignored no matter what's after it.

Example code:
```
my_func: # Adds two numbers together (REG1 = REG1 + REG2)
	add r1 r2
	mov r1 acc
	ret
_start:
	mov r1 5
	mov r2 10
	call @my_func
	jmp @_start
```

### Instructions
Assembly language defines the following instructions (VM CPUs must properly implement them):

Name | Instruction usage | Definition | Affected flags/Notes
---- | ---- | ---- | ----
MOV | MOV REG1 REG2 | REG1 = REG2 | Z
 | MOV REG VAL | REG = VAL | Z
ADD | ADD REG1 REG2 | ACC = REG1 + REG2 | CZ
ADC | ADC REG1 REG2 | ACC = REG1 + REG2 + (FLAGS[C] ? 1 : 0) | CZ
SUB | SUB REG1 REG2 | ACC = REG1 - REG2 | CZ
SBC | SBC REG1 REG2 | ACC = REG1 - REG2 - (FLAGS[C] ? 1 : 0) | CZ
CLF | | FLAGS=0 | Clear flags
LOAD | LOAD VAL | ACC = [DS:VAL] | Load a 32 bit value from the memory, addressed with DS + VAL
SAVE | SAVE REG | [DS:VAL] = REG | Save a 32 bit value from any register to address DS + VAL
CMP | CMP REG1 REG2 | Compare two register values, result is saved within flags | LZ
JMP | JMP VAL | | Jump to the target instruction number
JE | JE VAL | | Jump to the target instruction number (if Z flag is up)
JNE | JNE VAL | | Jump to the target instruction number (if Z flag is down)
JL | JL VAL | | Jump to the target instruction number (if L flag is up)
JLE | JLE VAL | | Jump to the target instruction number (if L or Z flag is up)
JG | JG VAL | | Jump to the target instruction number (if L and Z flags are down)
JGE | JGE VAL | | Jump to the target instruction number (if L flag is down)
CALL | CALL VAL | | Push the current instruction number onto the stack then jump to the target instruction number (procedures and functions)
RET | RET | | Pop an address from the stack then jump to it (end of procedures and functions)