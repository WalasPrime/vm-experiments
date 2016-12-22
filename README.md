# Virtual Machine Experiments [![Travis Buildstatus](https://travis-ci.org/WalasPrime/vm-experiments.svg)](https://travis-ci.org/WalasPrime/vm-experiments/)
Very simple Virtual Machines (custom architecture) written in many languages for benchmarking purposes. A small project made to help understand the practical side of simulating a real logical computer architecture (not a physical one, we don't simulate delays on the transistors).

## Building
This project uses SCons 2.5.0 as it's buildsystem and Python 2.7 as the testsuite. After cloning this repo just execute the `scons` command to build all the executables.
Test scripts are contained within `./scripts/testsuite`.

SCons Argument | Values | Meaning
---- | ---- | ----
DEBUG | 0,1 | Build machines with verbose output and additional checks

## Machines
Machine name | Description | Status | Docs
---- | ---- | ---- | ----
C | Pure C machine | On-hold, incomplete
Cpp | Mixed C/C++ machine | Arch-1 (not fully tested) | [Docs](doc/machines/cpp.md)
Cpp-ASM | Mixed C/C++/ASM (highly optimised) machine | Planned after architecture upgrades
Java | | Planned after architecture upgrades
JavaScript | Browserified Node machine | Planned after Node.js implementation
Node.js | JavaScript machine | Arch-1 (Work-in-progress)
PHP | | Planned after architecture upgrades

# About the machine

* [Assembly](#assembly-markdown)
* [Architectures](#architectures)
* [Testing](#testing)

## Plans
The following description is subject to change over time. The following changes (not limited to) will be introduced:
* Interrupt support (interrupt instructions, interrupt toggling, interrupt vector list)
* Data section in Assembly
* Some form of memory security that does not require memory segmenting
* Peripherals (simple I/O)

## Assembly markdown
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

### Architectures
Virtual CPUs must properly implement instruction specifications for the VM to be tagged as "arch-*".

Architecture name | Status | Docs
---- | ---- | ----
Architecture 1 | Work-in-progress | [Architecture 1](doc/arch/arch1.md)

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
