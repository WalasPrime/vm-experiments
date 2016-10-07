// C++ Implementation (note, right now this is based heavily on some unused C code)
// Principles:
//	- Heavy GOTO usage instead of functions

#include "util.h"
#include "arch.h"
#include "memory.h"

int main(){
	if(DEBUG_MODE == 0){
		printf("Machine in C++ starting\n");
	}else{
		printf("Machine in C++ starting DEBUG mode\n");
	}
	debug_printf("Memory manager, total addressable memory: %u bytes", VM_MAX_MEM_ADDR*4);
	debug_printf("Memory manager, segment size: %u bytes", VM_MGR_MEMORY_SEGMENT_SIZE*4);
	debug_printf("Memory manager, total memory segments: %u", VM_MGR_MEMORY_SEGMENT_COUNT);
	debug_printf("Memory manager size: %u bytes", (uint32_t)sizeof(vm_memory_controller));

	debug_printf("Memory manager is now created");
	vm_memory_controller* mem = new vm_memory_controller();

	debug_printf("CLEANING");
	delete mem;
	debug_printf("SAFE_END REACHED");
}