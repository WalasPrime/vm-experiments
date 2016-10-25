# Virtual Machine Experiments [![Travis Buildstatus](https://travis-ci.org/WalasPrime/vm-experiments.svg)](https://travis-ci.org/WalasPrime/vm-experiments/)
Very simple Virtual Machines (custom architecture) written in many languages for benchmarking purposes. A small project made to help understand the practical side of simulating a real logical computer architecture (not a physical one, we don't simulate delays on the transistors).

## Building
This project uses SCons 2.5.0 as it's buildsystem and Python 2.7 as the testsuite. After cloning this repo just execute the `scons` command to build all the executables.
Test scripts are contained within `./scripts/testsuite`.

SCons Argument | Values | Meaning
---- | ---- | ----
DEBUG | 0,1 | Build machines with verbose output and additional checks

## Current machines
Machine name | Description | Status
---- | ---- | ----
C | Pure C machine | On-hold, incomplete
Cpp | Mixed C/C++ machine | Work-in-progress
Cpp-ASM | Mixed C/C++/ASM (highly optimised) machine | Planned after architecture upgrades
Java | | Planned after architecture upgrades
JavaScript | Native JavaScript | Planned after architecture upgrades
Node.js | Encapsulated JavaScript machine | Planned after JavaScript implementation
PHP | | Planned after architecture upgrades

# About the machine

* [Machine architecture](#architecture)
* [Memory](#memory)
* [CPU registers](#cpu)
* [Assembly](#assembly-markdown)
* [Instructions](#instructions)
* [Testing](#testing)

## Architecture
The following description is subject to change over time. The following changes (not limited to) will be introduced over time:
* Interrupt support (interrupt instructions, interrupt toggling, interrupt vector list)
* Data section in Assembly
* Some form of memory security that does not require memory segmenting
* Peripherals (simple I/O)

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
L | Less flag - set if the result is negative, or CMP result "A < B"
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

### Testing
In order for machines to be testable each implementation has to:
* Allow executing different assembly programs (filename as argument)
* Allow saving memory dumps into external binary files (filename, address, range as argument)

To create a test:
* Create an assembly program, save it in `./programs` with `.asm` extension
* Create a memory dump (with machine arguments) and make sure the result is as expected (memory dumps contain 8 Bytes of location and size information at the begining)
* Put the memory dump in `./programs` under the same filename as the program name, but with `.memdump` extension
* Automatic tests in `./scripts/testsuite` will look for all `*.asm` files with corresponding `*.memdump` files, execute the programs on all machines and compare the memory dumps

### Memory dump files
Memory dump files (`.memdump`) are binary files of the following format:
Size | Content
---- | ----
4 Bytes | Memory location
4 Bytes | Dump length (dump_length)
(dump_length) Bytes | Binary memory content