#pragma once

#define VM_MAX_MEM_ADDR (0xFFFFFFFF-1)
#define VM_MGR_MEMORY_SEGMENT_SIZE (4096*100)
#define VM_MGR_MEMORY_SEGMENT_COUNT (VM_MAX_MEM_ADDR/VM_MGR_MEMORY_SEGMENT_SIZE+1)

#define VM_ADDR_SEGMENT(ADDR) (ADDR/VM_MGR_MEMORY_SEGMENT_SIZE)
#define VM_ADDR_SEG_LOCAL(ADDR) (ADDR-VM_ADDR_SEGMENT(ADDR)*VM_MGR_MEMORY_SEGMENT_SIZE)

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
		~vm_memory_controller(){
			for(uint32_t i = 0; i < VM_MGR_MEMORY_SEGMENT_COUNT; i++){
				if(this->segments[i] != NULL)
					delete this->segments[i];
			}
		}

		bool touch_address(uint32_t addr){
			uint32_t segment = VM_ADDR_SEGMENT(addr);
			if(this->segments[segment] == NULL){

			}
		}

		bool allocate_segment(uint32_t seg_num){
			vm_memory_segment* seg = new vm_memory_segment;
			if(seg){
				this->segments[seg_num] = seg;
				return true;
			}else{
				return false;
			}
		}

		bool write_address(uint32_t addr, uint32_t val){
			if(this->touch_address(addr)){
				this->segments[VM_ADDR_SEGMENT(addr)]->cell[VM_ADDR_SEG_LOCAL(addr)].mem = val;
				return true;
			}else{
				return false;
			}
		}

		bool read_address(uint32_t addr, uint32_t & dst){
			if(this->touch_address(addr)){
				dst = this->segments[VM_ADDR_SEGMENT(addr)]->cell[VM_ADDR_SEG_LOCAL(addr)].mem;
				return true;
			}else{
				return false;
			}
		}
};