#pragma once
#include "arch.h"
#include "util.h"

#define VM_MAX_MEM_ADDR (0xFFFFFFFF-1)
#define VM_MGR_MEMORY_SEGMENT_SIZE (4096*100)
#define VM_MGR_MEMORY_SEGMENT_COUNT (VM_MAX_MEM_ADDR/VM_MGR_MEMORY_SEGMENT_SIZE+1)

struct _vm_memory_segment {
	vm_memcell cell[VM_MGR_MEMORY_SEGMENT_SIZE];
};

#define vm_memory_segment struct _vm_memory_segment

struct _vm_memory_controller {
	vm_memory_segment* segments[VM_MGR_MEMORY_SEGMENT_COUNT];
};

#define vm_memory_controller struct _vm_memory_controller

void vm_mem_init(vm_memory_controller * self){
	for(uint32_t i = 0; i < VM_MGR_MEMORY_SEGMENT_COUNT; i++){
		debug_printf("Touching segment %u", i);
		self->segments[i] = NULL;
	}
}