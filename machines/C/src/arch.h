#pragma once
#include <stdint.h>

enum _vm_opcodes {
	_VM_INVALID_OPCODE_ = 0,

	VM_OPCODE_MOV,
	VM_OPCODE_ADD,
	VM_OPCODE_ADC,
	VM_OPCODE_SUB,
	VM_OPCODE_SBC,

	VM_OPCODE_CLF,
	VM_OPCODE_CMP,

	VM_OPCODE_LOAD,
	VM_OPCODE_SAVE,

	VM_OPCODE_JMP,
	VM_OPCODE_JE,
	VM_OPCODE_JNE,
	VM_OPCODE_JL,
	VM_OPCODE_JLE,
	VM_OPCODE_JG,
	VM_OPCODE_JGE,

	VM_OPCODE_CALL,
	VM_OPCODE_RET
};

const uint32_t _vm_opcode_length[] = {
	0, // _VM_INVALID_OPCODE_

	1, // VM_OPCODE_MOV
	1, // VM_OPCODE_ADD
	1, // VM_OPCODE_ADC
	1, // VM_OPCODE_SUB
	1, // VM_OPCODE_SBC

	1, // VM_OPCODE_CLF
	1, // VM_OPCODE_CMP

	2, // VM_OPCODE_LOAD
	2, // VM_OPCODE_SAVE

	2, // VM_OPCODE_JMP
	2, // VM_OPCODE_JE
	2, // VM_OPCODE_JNE
	2, // VM_OPCODE_JL
	2, // VM_OPCODE_JLE
	2, // VM_OPCODE_JG
	2, // VM_OPCODE_JGE

	2, // VM_OPCODE_CALL
	1  // VM_OPCODE_RET
};

struct _vm_instruction {
	uint8_t OPCODE;
	union args {
		struct t_reg2 {uint8_t REG1; uint8_t REG2;};
		struct t_reg_val {uint8_t REGx; uint32_t VAL;};
		uint8_t REG;
		uint32_t VALx;
	};
};

struct _vm_memcell {
	uint32_t mem;
};

struct _vm_memcell_ext {
	struct _vm_memcell mem[2];
};

#define vm_instruction struct _vm_instruction
#define vm_opcodes enum _vm_opcodes
#define vm_memcell struct _vm_memcell