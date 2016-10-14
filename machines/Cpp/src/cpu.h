#pragma once

#include "arch.h"
#include "memory.h"

#define VM_REG(VM_REGISTERS_NUM) (VM_REGISTERS_NUM-VM_REG32_ENUM_OFFSET)

struct _vm_cpu_state {
	// All 32 bit registers
	uint32_t reg[VM_REG32_COUNT];
};
typedef _vm_cpu_state vm_cpu_state;

class vm_cpu {
	private:
		vm_memory_controller* mem;
		vm_cpu_state state;
	public:

		vm_cpu(vm_memory_controller* pmem) : mem(pmem) {
			reset_state();
		}
		void reset_state(){
			memset(&state, 0, sizeof(state));
		}

		void execute_continuous(){
			// 1. Read opcode
			// 2. Deduce instruction format
			// 3. Load arguments*
			// 4. Execute
			// 5. Offset
			// 6. Goto 1
			vm_memcell_ext fetch;
			vm_instruction* instruction = (vm_instruction*)&fetch;

			fetch:
			if(!mem->read_address(state.reg[VM_REG(VM_REG_PC)], fetch.word[0].mem)){
				// FAILED TO READ A MEMORY CELL!
				debug_printf("CPU Failed reading memory cell addr %u for opcode fetch", state.reg[VM_REG(VM_REG_PC)]);
				return;
			}

			if(instruction->OPCODE >= _VM_OPCODE_LIMIT_){
				debug_printf("CPU Failed decoding instruction with opcode %u at %u", instruction->OPCODE, state.reg[VM_REG(VM_REG_PC)]);
				return;
			}
			if(instruction->OPCODE == _VM_INVALID_OPCODE_){
				debug_printf("CPU Decoded an invalid instruction code 0 at %u", state.reg[VM_REG(VM_REG_PC)]);
				return;
			}

			if(vm_opcode_length[instruction->OPCODE] > 1){
				if(!mem->read_address(state.reg[VM_REG(VM_REG_PC)]+1, fetch.word[1].mem)){
					debug_printf("CPU Failed reading memory cell addr %u for extended instruction fetch", state.reg[VM_REG(VM_REG_PC)]+1);
					return;
				}
			}

			switch(instruction->OPCODE){
				case VM_OPCODE_MOV: goto OP_MOV; break;
				case VM_OPCODE_CMOV: goto OP_CMOV; break;
				case VM_OPCODE_ADD: goto OP_ADD; break;

				case VM_OPCODE_JMP: goto OP_JMP; break;
				default:
					debug_printf("CPU Encountered an unimplemented instruction %u at %u", instruction->OPCODE, state.reg[VM_REG(VM_REG_PC)]);
					goto finish;
			}

			OP_MOV:
				if(instruction->REG1 == 0 || instruction->REG2 == 0 || instruction->REG1 > VM_REG32_COUNT-1 || instruction->REG2 > VM_REG32_COUNT-1){
					debug_printf("CPU MOV Invalid register argument at %u", state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				state.reg[VM_REG(instruction->REG1)] = state.reg[VM_REG(instruction->REG2)];
			goto finish;

			OP_CMOV:
				if(instruction->REG1 == 0 || instruction->REG1 > VM_REG32_COUNT-1){
					debug_printf("CPU CMOV Invalid register %u at %u", instruction->REG1, state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				state.reg[VM_REG(instruction->REG1)] = instruction->oVAL;
			goto finish;

			OP_ADD:
				if(instruction->REG1 == 0 || instruction->REG2 == 0 || instruction->REG1 > VM_REG32_COUNT-1 || instruction->REG2 > VM_REG32_COUNT-1){
					debug_printf("CPU ADD Invalid register argument at %u", state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				state.reg[VM_REG(VM_REG_ACC)] = state.reg[VM_REG(instruction->REG1)] + state.reg[VM_REG(instruction->REG2)];
				if(state.reg[VM_REG(instruction->REG1)] & VM_2POW32_BIT && state.reg[VM_REG(instruction->REG2)] & VM_2POW32_BIT){
					state.reg[VM_REG(VM_REG_FLAGS)] &= VM_FLAG_C;
				}else{
					state.reg[VM_REG(VM_REG_FLAGS)] &= ~VM_FLAG_C;
				}
			goto finish;

			OP_JMP:
				// FIXME: Jumps are compiled the wrong way at the moment
				debug_printf("CPU JMP at %u to %u", state.reg[VM_REG(VM_REG_PC)], instruction->VAL);
				state.reg[VM_REG(VM_REG_PC)] = instruction->VAL;
			goto fetch;

			finish:
				state.reg[VM_REG(VM_REG_PC)]+=vm_opcode_length[instruction->OPCODE];
			goto fetch;
		}
};