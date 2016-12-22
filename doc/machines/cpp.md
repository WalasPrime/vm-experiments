# Cpp machine
A VM implemented in C/C++11. Tries to minimise use of functions in favor of `goto`'s inside the CPU. Currently implements **Arch-1** specifications.

# Building
To build only this machine use `scons` inside of `machines/Cpp` directory. This will require a C++11 compatible compiler configured on your system. Debug build is also available.

# Running
Argument name | Description
---- | ----
-program | Load an assembly file, put it into memory and execute it
-memdump | After the first `BREAK` instruction make a `.memdump` file at this filename/path
-dumpaddr | Memory location where to start reading when doing the memory dumpaddr
-dumplength | How much memory to dump into the memdump

To properly make a memory dump provide all the dump arguments (`memdump dumpaddr dumplength`).

Example: `./c_machine -program ../../../programs/stack.asm -memdump my_dump.memdump -dumpaddr 1024 -dumplength 32` when inside of `machines/Cpp/bin` directory.

When writing a test program it's easier to stay at the project's root directory:
`machines/Cpp/bin/c_machine -program programs/my_test.asm -memdump programs/my_test.memdump -dumpaddr 1024 -dumplength 32` (change dumpaddr and dumplength to fit your program). This will generate a `.memdump` file that is compatible with the testsuite.

# Status codes
Status code | Situation
---- | ----
0 | A `BREAK` instruction was executed, or an execution error was encountered (invalid opcodes, invalid arguments)
-1 | A `FAIL` instruction was executed