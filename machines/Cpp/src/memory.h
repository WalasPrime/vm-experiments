#pragma once

#define VM_MAX_MEM_ADDR (0xFFFFFFFF-1)
#define VM_MGR_MEMORY_SEGMENT_SIZE (4096*100)
#define VM_MGR_MEMORY_SEGMENT_COUNT (VM_MAX_MEM_ADDR/VM_MGR_MEMORY_SEGMENT_SIZE+1)

struct _vm_memory_segment {
	vm_memcell cell[VM_MGR_MEMORY_SEGMENT_SIZE];
};

typedef _vm_memory_segment vm_memory_segment;

class vm_memory_controller {
	public:
		vm_memory_segment* segments[VM_MGR_MEMORY_SEGMENT_COUNT];

		vm_memory_controller(){
			for(uint32_t i = 0; i < VM_MGR_MEMORY_SEGMENT_COUNT; i++){
				this->segments[i] = NULL;
			}
		}
};