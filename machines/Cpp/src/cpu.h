#pragma once

#include "arch.h"
#include "memory.h"

#define VM_REG(VM_REGISTERS_NUM) (VM_REGISTERS_NUM-VM_REG32_ENUM_OFFSET)
#define REG_VARIANT_OP_CHECK(instruction) if(instruction->REG1 == 0 || instruction->REG1 > VM_REG32_COUNT-1 || ((instruction->OPTS & VM_OPT_VARIANT_REG) && (instruction->REG2 == 0 || instruction->REG2 > VM_REG32_COUNT-1)))
#define REG_OP_CHECK(reg) if(reg == 0 || reg > VM_REG32_COUNT-1)

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
				//case VM_OPCODE_CMOV: goto OP_CMOV; break;
				case VM_OPCODE_ADD: goto OP_ADD; break;
				case VM_OPCODE_ADC: goto OP_ADC; break;
				case VM_OPCODE_SUB: goto OP_SUB; break;
				// TODO: case VM_OPCODE_SBC: goto OP_SBC; break;

				case VM_OPCODE_CLF: goto OP_CLF; break;
				case VM_OPCODE_CMP: goto OP_CMP; break;

				case VM_OPCODE_LOAD: goto OP_LOAD; break;
				case VM_OPCODE_SAVE: goto OP_SAVE; break;

				case VM_OPCODE_JE: goto OP_JE; break;
				case VM_OPCODE_JNE: goto OP_JNE; break;
				case VM_OPCODE_JL: goto OP_JL; break;
				case VM_OPCODE_JLE: goto OP_JLE; break;
				case VM_OPCODE_JG: goto OP_JG; break;
				case VM_OPCODE_JGE: goto OP_JGE; break;
				case VM_OPCODE_JMP: goto OP_JMP; break;

				case VM_OPCODE_PUSH: goto OP_PUSH; break;
				case VM_OPCODE_POP: goto OP_POP; break;
				case VM_OPCODE_CALL: goto OP_CALL; break;
				case VM_OPCODE_RET: goto OP_RET; break;
				// TODO: BREAK
				default:
					debug_printf("CPU Encountered an unimplemented instruction %u at %u", instruction->OPCODE, state.reg[VM_REG(VM_REG_PC)]);
					goto finish;
			}

			OP_MOV:
				REG_VARIANT_OP_CHECK(instruction){
					debug_printf("CPU MOV Invalid register argument at %u", state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				state.reg[VM_REG(instruction->REG1)] = instruction->oVAL;
				if(instruction->OPTS & VM_OPT_VARIANT_REG)
					state.reg[VM_REG(instruction->REG1)] = state.reg[VM_REG(instruction->REG2)];

				if(state.reg[VM_REG(instruction->REG1)] == 0){
					state.reg[VM_REG(VM_REG_FLAGS)] |= VM_FLAG_Z;
				}else{
					state.reg[VM_REG(VM_REG_FLAGS)] &= ~VM_FLAG_Z;
				}
			goto finish;

			/*OP_CMOV:
				if(instruction->REG1 == 0 || instruction->REG1 > VM_REG32_COUNT-1){
					debug_printf("CPU CMOV Invalid register %u at %u", instruction->REG1, state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				state.reg[VM_REG(instruction->REG1)] = instruction->oVAL;
				if(state.reg[VM_REG(instruction->REG1)] == 0){
					state.reg[VM_REG(VM_REG_FLAGS)] |= VM_FLAG_Z;
				}else{
					state.reg[VM_REG(VM_REG_FLAGS)] &= ~VM_FLAG_Z;
				}
			goto finish;*/

			OP_ADD:
				REG_VARIANT_OP_CHECK(instruction){
					debug_printf("CPU ADD Invalid register argument at %u", state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				state.reg[VM_REG(VM_REG_ACC)] = state.reg[VM_REG(instruction->REG1)] + state.reg[VM_REG(instruction->REG2)];
				if(state.reg[VM_REG(instruction->REG1)] & VM_2POW32_BIT && state.reg[VM_REG(instruction->REG2)] & VM_2POW32_BIT){
					state.reg[VM_REG(VM_REG_FLAGS)] |= VM_FLAG_C;
				}else{
					state.reg[VM_REG(VM_REG_FLAGS)] &= ~VM_FLAG_C;
				}
				if(state.reg[VM_REG(VM_REG_ACC)] == 0){
					state.reg[VM_REG(VM_REG_FLAGS)] |= VM_FLAG_Z;
				}else{
					state.reg[VM_REG(VM_REG_FLAGS)] &= ~VM_FLAG_Z;
				}
			goto finish;

			OP_ADC:
				REG_VARIANT_OP_CHECK(instruction){
					debug_printf("CPU ADC Invalid register argument at %u", state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				// FIXME: Nasty ADD copypasta
				state.reg[VM_REG(VM_REG_ACC)] = state.reg[VM_REG(instruction->REG1)] + state.reg[VM_REG(instruction->REG2)] + (state.reg[VM_REG(VM_REG_FLAGS)] & VM_FLAG_C);
				if(state.reg[VM_REG(instruction->REG1)] & VM_2POW32_BIT && state.reg[VM_REG(instruction->REG2)] & VM_2POW32_BIT){
					state.reg[VM_REG(VM_REG_FLAGS)] |= VM_FLAG_C;
				}else{
					state.reg[VM_REG(VM_REG_FLAGS)] &= ~VM_FLAG_C;
				}
				if(state.reg[VM_REG(VM_REG_ACC)] == 0){
					state.reg[VM_REG(VM_REG_FLAGS)] |= VM_FLAG_Z;
				}else{
					state.reg[VM_REG(VM_REG_FLAGS)] &= ~VM_FLAG_Z;
				}
			goto finish;

			OP_SUB:
				REG_VARIANT_OP_CHECK(instruction){
					debug_printf("CPU SUB Invalid register argument at %u", state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				state.reg[VM_REG(VM_REG_ACC)] = (uint32_t)((int32_t)state.reg[VM_REG(instruction->REG1)] - (int32_t)state.reg[VM_REG(instruction->REG2)]);
				// TODO: Overflow flags
				// TODO: Carry flag
				// FIXME: ADD copypasta
				if(state.reg[VM_REG(VM_REG_ACC)] == 0){
					state.reg[VM_REG(VM_REG_FLAGS)] |= VM_FLAG_Z;
				}else{
					state.reg[VM_REG(VM_REG_FLAGS)] &= ~VM_FLAG_Z;
				}
			goto finish;

			/*
			OP_SBC:

			goto finish;
			*/

			OP_CLF:
				state.reg[VM_REG(VM_REG_FLAGS)] = 0;
			goto finish;

			OP_CMP:
				REG_VARIANT_OP_CHECK(instruction){
					debug_printf("CPU CMP Invalid register argument at %u", state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				state.reg[VM_REG(VM_REG_FLAGS)] &= ~(VM_FLAG_L | VM_FLAG_Z);
				if(state.reg[VM_REG(instruction->REG1)] < state.reg[VM_REG(instruction->REG2)]){
					state.reg[VM_REG(VM_REG_FLAGS)] |= VM_FLAG_L;
				}
				if(state.reg[VM_REG(instruction->REG1)] == state.reg[VM_REG(instruction->REG2)]){
					state.reg[VM_REG(VM_REG_FLAGS)] |= VM_FLAG_Z;
				}
			goto finish;

			OP_LOAD:
				REG_VARIANT_OP_CHECK(instruction){
					debug_printf("CPU LOAD Invalid register argument at %u", state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				if(!mem->read_address(instruction->VAL, state.reg[VM_REG(instruction->REG1)])){
					debug_printf("CPU LOAD failed reading address %u at %u", instruction->VAL, state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
			goto finish;

			OP_SAVE:
				REG_VARIANT_OP_CHECK(instruction){
					debug_printf("CPU SAVE Invalid register argument at %u", instruction->REG1);
					return;
				}
				if(!mem->write_address(instruction->VAL, state.reg[VM_REG(instruction->REG1)])){
					debug_printf("CPU SAVE failed writing address %u at %u", instruction->VAL, state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
			goto finish;

			OP_JE:
				if(state.reg[VM_REG(VM_REG_PC)] & VM_FLAG_Z != 0)
					goto OP_JMP;
			goto finish;

			OP_JNE:
				if(state.reg[VM_REG(VM_REG_PC)] & VM_FLAG_Z == 0)
					goto OP_JMP;
			goto finish;

			OP_JL:
				if(state.reg[VM_REG(VM_REG_PC)] & VM_FLAG_L != 0)
					goto OP_JMP;
			goto finish;

			OP_JLE:
				if(state.reg[VM_REG(VM_REG_PC)] & (VM_FLAG_L | VM_FLAG_Z) != 0)
					goto OP_JMP;
			goto finish;

			OP_JG:
				if(state.reg[VM_REG(VM_REG_PC)] & (VM_FLAG_L | VM_FLAG_Z) == 0)
					goto OP_JMP;
			goto finish;

			OP_JGE:
				if(state.reg[VM_REG(VM_REG_PC)] & (VM_FLAG_L) == 0)
					goto OP_JMP;
			goto finish;

			OP_JMP:
				state.reg[VM_REG(VM_REG_PC)] = instruction->oVAL;
				if(instruction->OPTS & VM_OPT_VARIANT_REG){
					if(REG_OP_CHECK(instruction->REG2)){
						state.reg[VM_REG(VM_REG_PC)] = instruction->REG2;
					}else{
						debug_printf("CPU JMP Invalid register at %u", state.reg[VM_REG(VM_REG_PC)]);
						return;
					}
				}
			goto fetch;

			OP_PUSH:
				REG_OP_CHECK(instruction->REG1){
					debug_printf("CPU PUSH Invalid register at %u", state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				// FIXME: OP_CALL copypasta
				if(!mem->write_address(state.reg[VM_REG(instruction->REG1)], state.reg[VM_REG(VM_REG_SP)]+state.reg[VM_REG(VM_REG_SS)])){
					debug_printf("CPU PUSH failed pushing registry to address %u at %u", state.reg[VM_REG(VM_REG_SP)]+state.reg[VM_REG(VM_REG_SS)], state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				state.reg[VM_REG(VM_REG_SP)]++;
			goto fetch;

			OP_POP:
				REG_OP_CHECK(instruction->REG1){
					debug_printf("CPU POP Invalid register at %u", state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				// FIXME: OP_RET copypasta
				if(state.reg[VM_REG(VM_REG_SP)] == 0){
					debug_printf("CPU POP failed, stack pointer zero at %u", state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				if(!mem->read_address(state.reg[VM_REG(instruction->REG1)]+state.reg[VM_REG(VM_REG_SP)], instruction->VAL)){
					debug_printf("CPU POP failed reading stack entry %u at %u", state.reg[VM_REG(VM_REG_SS)]+state.reg[VM_REG(VM_REG_SP)], state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				state.reg[VM_REG(VM_REG_SP)]--;
			goto fetch;

			OP_CALL:
				// PUSH, JMP
				if(!mem->write_address(state.reg[VM_REG(VM_REG_PC)], state.reg[VM_REG(VM_REG_SP)]+state.reg[VM_REG(VM_REG_SS)])){
					debug_printf("CPU CALL failed pushing PC to address %u at %u", state.reg[VM_REG(VM_REG_SP)]+state.reg[VM_REG(VM_REG_SS)], state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				state.reg[VM_REG(VM_REG_SP)]++;
			goto OP_JMP;

			OP_RET:
				// POP, JMP
				if(state.reg[VM_REG(VM_REG_SP)] == 0){
					debug_printf("CPU RET failed, stack pointer zero at %u", state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				if(!mem->read_address(state.reg[VM_REG(VM_REG_SS)]+state.reg[VM_REG(VM_REG_SP)], instruction->oVAL)){
					debug_printf("CPU RET failed reading stack entry %u at %u", state.reg[VM_REG(VM_REG_SS)]+state.reg[VM_REG(VM_REG_SP)], state.reg[VM_REG(VM_REG_PC)]);
					return;
				}
				state.reg[VM_REG(VM_REG_SP)]--;
			goto OP_JMP;

			finish:
				state.reg[VM_REG(VM_REG_PC)]+=vm_opcode_length[instruction->OPCODE];
			goto fetch;
		}
};