#pragma once

#include <iomanip>

#include "util.h"

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
			if(segment >= VM_MGR_MEMORY_SEGMENT_COUNT){
				debug_printf("MEMORY Segmentation logic error while evaluating address %u", addr);
				return false;
			}
			if(this->segments[segment] == NULL){
				return this->allocate_segment(segment);
			}
			return true;
		}

		bool allocate_segment(uint32_t seg_num){
			vm_memory_segment* seg = new vm_memory_segment;
			if(seg){
				debug_printf("Allocated segment %u (%u bytes)", seg_num, VM_MGR_MEMORY_SEGMENT_SIZE);
				this->segments[seg_num] = seg;
				memset(seg, 0, sizeof(vm_memory_segment));
				return true;
			}else{
				debug_printf("Failed to allocate segment %u (%u bytes)", seg_num, VM_MGR_MEMORY_SEGMENT_SIZE);
				return false;
			}
		}

		bool write_address(uint32_t addr, uint32_t val){
			if(this->touch_address(addr)){
				this->segments[VM_ADDR_SEGMENT(addr)]->cell[VM_ADDR_SEG_LOCAL(addr)].mem = val;
				debug_printf("MEM WRITE %u at %u", val, addr);
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

		bool write_address_ext(uint32_t addr, uint32_t* data, uint8_t count){
			for(uint8_t i = 0; i < count; i++)
				if(!this->write_address(addr+i, data[i]))
					return false;
			return true;
		}

		bool read_address_ext(uint32_t addr, uint32_t* target, uint8_t count){
			for(uint8_t i = 0; i < count; i++)
				if(!this->read_address(addr+i, target[i]))
					return false;
			return true;
		}

		void dump_mem(uint32_t const& addr_from, uint32_t const& length){
			std::cout << std::setfill('0') << std::setw(2);
			uint32_t val;
			vm_memcell8* val8;
			val8 = (vm_memcell8*)&val;
			for(uint32_t i = addr_from; i < addr_from+length; i++){
				this->read_address(i, val);
				std::cout << std::dec << i << std::hex << ": " << (uint32_t)val8->mem[0] << ":" << (uint32_t)val8->mem[1] << ":" << (uint32_t)val8->mem[2] << ":" << (uint32_t)val8->mem[3];
				if(val8->mem[0] > _VM_INVALID_OPCODE_ && val8->mem[0] < _VM_OPCODE_LIMIT_){
					std::cout << "  (OP: " << std::dec << (uint32_t)val8->mem[0] << ")";
				}
				std::cout << std::endl;
			}
			std::cout << std::dec << std::endl;
		}
};