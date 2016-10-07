#include <stdio.h>

// Pure C implementation
// Principles:
//	- GOTO's used as the main flow control of VM execution

#include "util.h"
#include "arch.h"
#include "memory.h"

int main(){
	vm_memory_controller* MEM = 0;

	debug_printf("START");
	if(DEBUG_MODE == 0){
		printf("Machine in C starting\n");
	}else{
		printf("Machine in C starting DEBUG mode\n");
	}
	debug_printf("Memory manager, total addressable memory: %u bytes", VM_MAX_MEM_ADDR*4);
	debug_printf("Memory manager, segment size: %u bytes", VM_MGR_MEMORY_SEGMENT_SIZE*4);
	debug_printf("Memory manager, total memory segments: %u", VM_MGR_MEMORY_SEGMENT_COUNT);
	debug_printf("Memory manager size: %u bytes", (uint32_t)sizeof(vm_memory_controller));
	
	float* X = (float*)malloc(16);
	*X = 15;

	debug_printf("ENDING");

	debug_printf("SAFE_END");
}