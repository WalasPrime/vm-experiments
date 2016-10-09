// C++ Implementation (note, right now this is based heavily on some unused C code)
// Principles:
//	- No special optimisations should be used here
//	- This is the "it just works" machine

#include "util.h"
#include "arch.h"
#include "memory.h"
#include "assembler_loader.h"

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

	asm_loader asm_ldr;
	if(!asm_ldr.parse_file("programs/test_basic.asm")){
		std::cout << "Error while compiling the program: " << std::endl << asm_ldr.last_error_str << " at line " << asm_ldr.last_error_line << " (col " << asm_ldr.last_error_col << ")" << std::endl;
		std::cout << asm_ldr.last_error_line_content << std::endl;
		std::cout << std::string(asm_ldr.last_error_col, ' ') << "^" << std::endl;
		std::cout << std::string(asm_ldr.last_error_col, ' ') << asm_ldr.last_error_line_content[asm_ldr.last_error_col] << std::endl;
		goto fail_end;
	}else{
		debug_printf("Program compiled succesfully");
	}

	debug_printf("CLEANING");
	delete mem;
	debug_printf("SAFE_END REACHED");
	return 0;

	fail_end:
	return -1;
}