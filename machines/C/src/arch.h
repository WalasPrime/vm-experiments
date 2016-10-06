#include <stdint.h>

struct _vm_instruction {
	int8_t OPCODE;
};

#define vm_instruction struct _vm_instruction