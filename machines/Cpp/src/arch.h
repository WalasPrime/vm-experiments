#pragma once
#include <stdint.h>

enum _vm_opcodes {
	_VM_INVALID_OPCODE_ = 0,

	VM_OPCODE_MOV,
	//VM_OPCODE_CMOV,
	VM_OPCODE_ADD,
	VM_OPCODE_ADC,
	VM_OPCODE_INC,
	VM_OPCODE_SUB,
	VM_OPCODE_SBC,
	VM_OPCODE_DEC,

	VM_OPCODE_CLF,
	VM_OPCODE_CMP,

	VM_OPCODE_LOAD,
	VM_OPCODE_SAVE,

	VM_OPCODE_JE,
	VM_OPCODE_JNE,
	VM_OPCODE_JL,
	VM_OPCODE_JLE,
	VM_OPCODE_JG,
	VM_OPCODE_JGE,
	VM_OPCODE_JMP,

	VM_OPCODE_PUSH,
	VM_OPCODE_POP,
	VM_OPCODE_CALL,
	VM_OPCODE_RET,
	VM_OPCODE_BREAK,
	VM_OPCODE_FAIL,

	_VM_OPCODE_LIMIT_
};

const uint32_t vm_opcode_length[] = {
	0, // _VM_INVALID_OPCODE_

	2, // VM_OPCODE_MOV
	//2, // VM_OPCODE_CMOV
	2, // VM_OPCODE_ADD
	2, // VM_OPCODE_ADC
	1, // VM_OPCODE_INC
	2, // VM_OPCODE_SUB
	2, // VM_OPCODE_SBC
	2, // VM_OPCODE_DEC

	1, // VM_OPCODE_CLF
	2, // VM_OPCODE_CMP

	2, // VM_OPCODE_LOAD
	2, // VM_OPCODE_SAVE

	2, // VM_OPCODE_JE
	2, // VM_OPCODE_JNE
	2, // VM_OPCODE_JL
	2, // VM_OPCODE_JLE
	2, // VM_OPCODE_JG
	2, // VM_OPCODE_JGE
	2, // VM_OPCODE_JMP

	1, // VM_OPCODE_PUSH
	1, // VM_OPCODE_POP
	2, // VM_OPCODE_CALL
	1, // VM_OPCODE_RET
	1, // VM_OPCODE_BREAK
	1 // VM_OPCODE_FAIL 
};

enum _vm_registers {
	_VM_INVALID_REG_ = 0,

	VM_REG_PC,
	VM_REG_FLAGS,
	VM_REG_ACC,
	VM_REG_DS,
	VM_REG_SS,
	VM_REG_SP,
	VM_REG_R0,
	VM_REG_R1,
	VM_REG_R2,
	VM_REG_R3,
	VM_REG_R4,
	VM_REG_R5,
	VM_REG_R6,
	VM_REG_R7,
	VM_REG_R8,
	VM_REG_R9,
	VM_REG_R10,
	VM_REG_R11
};

enum _vm_flags {
	VM_FLAG_C = 1,
	VM_FLAG_Z = 2,
	VM_FLAG_L = 4
};

enum _vm_opts {
	VM_OPT_VARIANT_REG = 1 // up if variant is REG2 rather than oVAL
};
#define VM_2POW32_BIT (1<<31)

#define VM_REG32_COUNT 18
#define VM_REG32_ENUM_OFFSET 1

#pragma pack(push, 1)
struct _vm_instruction {
	uint8_t OPCODE;
	uint8_t OPTS;
	union {
		struct {uint8_t REG1; uint8_t REG2;};
		struct {uint8_t __unused1; uint32_t oVAL;}; // offsetValue
		uint32_t VAL;
	};
};
#pragma pack(pop)

struct _vm_memcell {
	uint32_t mem;
};

struct _vm_memcell8 {
	uint8_t mem[4];
};

struct _vm_memcell_ext {
	struct _vm_memcell word[2];
};

typedef _vm_instruction vm_instruction;
typedef _vm_opcodes vm_opcodes;
typedef _vm_memcell vm_memcell;
typedef _vm_memcell_ext vm_memcell_ext;
typedef _vm_registers vm_registers;
typedef _vm_flags vm_flags;
typedef _vm_memcell8 vm_memcell8;