#include <stdio.h>

// Pure C implementation
// Principles:
//	- GOTO's used as JMP's
//	- Compiler and the execution mechanism must be contained within the same function for GOTO's to work

#include "util.h"

int main(){
	debug_printf("START");
	if(DEBUG_MODE == 0){
		printf("Machine in C starting\n");
	}else{
		printf("Machine in C starting DEBUG mode\n");
	}
	
	
	
	debug_printf("SAFE_END");
}