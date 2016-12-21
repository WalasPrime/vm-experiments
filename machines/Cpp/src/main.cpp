// C++ Implementation (note, right now this is based heavily on some unused C code)
// Principles:
//	- No special optimisations should be used here
//	- This is the "it just works" machine

#include <fstream>

#include "util.h"
#include "arch.h"
#include "memory.h"
#include "assembler_loader.h"
#include "cpu.h"
#include "argv.h"

int main(int argc, char* argv[]){
	process_arguments(argc, argv);
	if(ARG_PROGRAM.empty()){
		printf("Machine requires a program to run (-program option)\n");
		return 1;
	}

	if(DEBUG_MODE == 0){
		printf("Machine in C++ starting\n");
	}else{
		printf("Machine in C++ starting DEBUG mode\n");
	}
	debug_cout("Will run program " << ARG_PROGRAM);
	if(ARG_DUMP_PATH.empty()){
		debug_cout("No memory dump will be created");
	}else{
		debug_cout("A memory dump will be created at " << ARG_DUMP_PATH << " of area " << ARG_DUMP_OFFSET << " - " << (ARG_DUMP_OFFSET+ARG_DUMP_LENGTH));
	}
	debug_printf("Memory manager, total addressable memory: %u bytes", VM_MAX_MEM_ADDR*4);
	debug_printf("Memory manager, segment size: %u bytes", VM_MGR_MEMORY_SEGMENT_SIZE*4);
	debug_printf("Memory manager, total memory segments: %u", VM_MGR_MEMORY_SEGMENT_COUNT);
	debug_printf("Memory manager size: %u bytes", (uint32_t)sizeof(vm_memory_controller));

	debug_printf("Memory manager is now created");
	vm_memory_controller* mem = new vm_memory_controller();

	asm_loader asm_ldr;
	if(!asm_ldr.parse_file(ARG_PROGRAM, mem)){
		std::cout << "Error while compiling the program: " << std::endl << asm_ldr.last_error_str << " at line " << asm_ldr.last_error_line << " (col " << asm_ldr.last_error_col << ")" << std::endl;
		std::cout << asm_ldr.last_error_line_content << std::endl;
		std::cout << std::string(asm_ldr.last_error_col, ' ') << "^" << std::endl;
		std::cout << std::string(asm_ldr.last_error_col, ' ') << asm_ldr.last_error_line_content[asm_ldr.last_error_col] << std::endl;
		return -1;
	}else{
		debug_printf("Program compiled succesfully");
	}

	vm_cpu exec(mem);
	exec.execute_continuous();
	if(exec.state.status == VM_STATUS_FAILED){
		std::cout << "Program FAILED at " << exec.state.reg[VM_REG(VM_REG_PC)] << std::endl;
		// TODO: Better dump with register names?
		for(uint32_t i = 0; i < VM_REG32_COUNT; i++){
			std::cout << "CPU.REG[" << i << "] = " << std::hex << exec.state.reg[i] << std::dec << "\t\t";
			if(i != 0 && (i % 4 == 0))
				std::cout << std::endl;
		}
		return -1;
	}

	if(!ARG_DUMP_PATH.empty()){
		debug_printf("Generating a memory dump file");
		std::ofstream dump (ARG_DUMP_PATH.c_str(), std::ofstream::binary);
		dump.write((char*)&ARG_DUMP_OFFSET, sizeof(ARG_DUMP_OFFSET));
		dump.write((char*)&ARG_DUMP_LENGTH, sizeof(ARG_DUMP_LENGTH));
		uint32_t temp;
		for(uint32_t at = ARG_DUMP_OFFSET; at < ARG_DUMP_OFFSET+ARG_DUMP_LENGTH; at++){
			if(!mem->read_address(at, temp)){
				printf("Error while generating a memory dump at mem %u\n", at);
				break;
			}
			dump.write((char*)&temp, sizeof(temp));
		}
		debug_printf("Finished generating a dump file, closing");
		dump.close();
	}

	debug_printf("CLEANING");
	delete mem;
	debug_printf("SAFE_END REACHED");
	return 0;
}