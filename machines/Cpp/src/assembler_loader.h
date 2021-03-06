#pragma once
#include <fstream>
#include <stdlib.h>
#include <ctype.h>
#include <cstring>
#include <vector>
#include <map>

#include "util.h"
#include "memory.h"

class asm_loader_raw_instruction {
	public:
		std::string INSTRUCTION_NAME;
		std::vector<std::string> INSTRUCTION_ARGS;

		std::string raw_line;
		uint16_t line_num;

		bool written_to_mem;
		uint32_t written_mem_addr;

		asm_loader_raw_instruction(std::string const& name, std::string const& line, uint16_t const& line_n) : INSTRUCTION_NAME(name), raw_line(line), line_num(line_n) {
			written_to_mem = false;
			written_mem_addr = 0;
		}

		void add_arg(std::string const& arg){
			INSTRUCTION_ARGS.push_back(arg);
		}
};

class asm_loader_instruction_store {
	public:
		std::vector<asm_loader_raw_instruction> instructions;
		std::map<std::string, uint32_t> etiquettes;

		void add_new_instruction(std::string const& name, std::string const& raw_line, uint16_t const& line_num){
			instructions.push_back(asm_loader_raw_instruction(name, raw_line, line_num));
		}

		void etiquette_next_instruction(std::string const& name){
			etiquettes[name] = instructions.size();
		}

		bool is_etiquette_defined(std::string const& name){
			return (etiquettes.find(name) != etiquettes.end());
		}

		uint32_t get_etiquette_addr(std::string const& name){
			return instructions[etiquettes[name]].written_mem_addr;
		}
		asm_loader_raw_instruction* get_etiquette_instruction(std::string const& name){
			return &instructions[etiquettes[name]];
		}

		void append_arg_to_last_instruction(std::string const& arg){
			instructions[instructions.size()-1].add_arg(arg);
		}

		asm_loader_raw_instruction* get_instruction(uint32_t const& number){
			if(number < instructions.size())
				return &instructions[number];
			return NULL;
		}
};

class asm_loader {
	private:
		enum PARSER_STATES {
			NO_STATE,

			STATE_FETCHING_INITIAL_WORD,	// Reading some characters for the first word
			STATE_EXPECT_EOL,				// Excepting \n at the next character
			STATE_FETCHING_ARGUMENTS,		// Reading characters for arguments
			STATE_ARGUMENTS_SPACES			// Reading spaces between arguments
		};
		enum OPCODE_INSTRUCTION_FORMATS {
			_OPCODE_INVALID_FORMAT_ = 0,
			OPCODE_NO_ARGUMENTS,
			OPCODE_VALUE_32BIT,
			OPCODE_REG,
			OPCODE_REG_VALUE_32BIT,
			OPCODE_REG_REG,
			OPCODE_REG_VARIANT,
			OPCODE_VARIANT
		};
		typedef struct _opcode_dictionary {
			std::string identifier;
			OPCODE_INSTRUCTION_FORMATS format;
			vm_opcodes code;

			_opcode_dictionary(){identifier="__NAN__"; format = OPCODE_NO_ARGUMENTS;code = _VM_INVALID_OPCODE_;}
			_opcode_dictionary(std::string const& n, OPCODE_INSTRUCTION_FORMATS const& f, vm_opcodes const& c) : identifier(n), format(f), code(c) {}
		} opcode_dictionary_str;
		typedef struct _reg_dictionary {
			std::string identifier;
			vm_registers code;

			_reg_dictionary(){identifier="__NAN_REG__"; code = _VM_INVALID_REG_;}
			_reg_dictionary(std::string const& n, vm_registers const& c) : identifier(n), code(c) {}
		} reg_dictionary_str;

		PARSER_STATES primary_state;
	public:
		std::string last_error_str;
		uint16_t last_error_line;
		uint16_t last_error_col;
		std::string last_error_line_content;

		std::map<std::string, opcode_dictionary_str> op_dictionary;
		std::map<std::string, reg_dictionary_str> reg_dictionary;

		asm_loader(){
			primary_state = NO_STATE;
			last_error_str = "";
			last_error_line_content = "";
			last_error_line = 0;
			last_error_col = 0;

			add_op_dict("MOV", OPCODE_REG_VARIANT, VM_OPCODE_MOV);
			//add_op_dict("CMOV", OPCODE_REG_VALUE_32BIT, VM_OPCODE_CMOV);
			add_op_dict("ADD", OPCODE_REG_VARIANT, VM_OPCODE_ADD);
			add_op_dict("ADC", OPCODE_REG_VARIANT, VM_OPCODE_ADC);
			add_op_dict("INC", OPCODE_REG, VM_OPCODE_INC);
			add_op_dict("SUB", OPCODE_REG_VARIANT, VM_OPCODE_SUB);
			add_op_dict("SBC", OPCODE_REG_VARIANT, VM_OPCODE_SBC);
			add_op_dict("DEC", OPCODE_REG, VM_OPCODE_DEC);
			add_op_dict("CLF", OPCODE_NO_ARGUMENTS, VM_OPCODE_CLF);
			add_op_dict("LOAD", OPCODE_REG_VARIANT, VM_OPCODE_LOAD);
			add_op_dict("SAVE", OPCODE_REG_VARIANT, VM_OPCODE_SAVE);
			add_op_dict("CMP", OPCODE_REG_VARIANT, VM_OPCODE_CMP);
			add_op_dict("JMP", OPCODE_VARIANT, VM_OPCODE_JMP);
			add_op_dict("JE", OPCODE_VARIANT, VM_OPCODE_JE);
			add_op_dict("JNE", OPCODE_VARIANT, VM_OPCODE_JNE);
			add_op_dict("JL", OPCODE_VARIANT, VM_OPCODE_JL);
			add_op_dict("JLE", OPCODE_VARIANT, VM_OPCODE_JLE);
			add_op_dict("JG", OPCODE_VARIANT, VM_OPCODE_JG);
			add_op_dict("JGE", OPCODE_VARIANT, VM_OPCODE_JGE);
			add_op_dict("PUSH", OPCODE_REG, VM_OPCODE_PUSH);
			add_op_dict("POP", OPCODE_REG, VM_OPCODE_POP);
			add_op_dict("CALL", OPCODE_VARIANT, VM_OPCODE_CALL);
			add_op_dict("RET", OPCODE_NO_ARGUMENTS, VM_OPCODE_RET);
			add_op_dict("BREAK", OPCODE_NO_ARGUMENTS, VM_OPCODE_BREAK);
			add_op_dict("FAIL", OPCODE_NO_ARGUMENTS, VM_OPCODE_FAIL);

			add_reg_dict("PC", VM_REG_PC);
			add_reg_dict("FLAGS", VM_REG_FLAGS);
			add_reg_dict("ACC", VM_REG_ACC);
			add_reg_dict("DS", VM_REG_DS);
			add_reg_dict("SS", VM_REG_SS);
			add_reg_dict("SP", VM_REG_SP);
			add_reg_dict("R0", VM_REG_R0);
			add_reg_dict("R1", VM_REG_R1);
			add_reg_dict("R2", VM_REG_R2);
			add_reg_dict("R3", VM_REG_R3);
			add_reg_dict("R4", VM_REG_R4);
			add_reg_dict("R5", VM_REG_R5);
			add_reg_dict("R6", VM_REG_R6);
			add_reg_dict("R7", VM_REG_R7);
			add_reg_dict("R8", VM_REG_R8);
			add_reg_dict("R9", VM_REG_R9);
			add_reg_dict("R10", VM_REG_R10);
			add_reg_dict("R11", VM_REG_R11);
		}

		OPCODE_INSTRUCTION_FORMATS get_instruction_format(std::string const& name){
			if(op_dictionary.find(name) != op_dictionary.end()){
				return op_dictionary[name].format;
			}else{
				return _OPCODE_INVALID_FORMAT_;
			}
		}

		vm_opcodes get_instruction_opcode(std::string const& name){
			if(op_dictionary.find(name) != op_dictionary.end()){
				return op_dictionary[name].code;
			}else{
				return _VM_INVALID_OPCODE_;
			}
		}

		vm_registers get_register_code(std::string const& name){
			if(reg_dictionary.find(name) != reg_dictionary.end()){
				return reg_dictionary[name].code;
			}else{
				return _VM_INVALID_REG_;
			}
		}

		void add_op_dict(std::string const& asm_code, OPCODE_INSTRUCTION_FORMATS const& format, vm_opcodes const& code){
			op_dictionary[asm_code] = opcode_dictionary_str(asm_code, format, code);
		}

		void add_reg_dict(std::string const& asm_code, vm_registers const& code){
			reg_dictionary[asm_code] = reg_dictionary_str(asm_code, code);
		}

		void errorize(std::string raw_line, std::string description, uint16_t line_num, uint16_t column){
			last_error_str = description;
			last_error_line = line_num;
			last_error_col = column;
			last_error_line_content = raw_line;
		}

		bool parse_file(std::string path, vm_memory_controller* mem){
			std::ifstream f(path.c_str());
			if(!f.is_open()){
				debug_cout("File " << path << " could not be opened for compiling");
				return false;
			}
			debug_cout("Opened " << path << " for compilation");

			asm_loader_instruction_store instructions;
			// FIRST PASS - Load lines, convert to an array of simple instructions
			char LINE[512];
			uint16_t current_line = 1;
			while(!f.eof()){
				f.getline(LINE, 512);

				uint16_t cursor_pos = 0;
				uint16_t line_length = strlen(LINE);

				debug_cout("LOADED LINE: " << std::string(LINE) << " (length " << line_length << ")");

				#define ERROR(STR) {errorize(std::string(LINE), "SYNTAX ERROR: " STR, current_line, cursor_pos); return false;}

				std::string current_word = "";
				primary_state = NO_STATE;
				while(cursor_pos <= line_length){
					nextchar_noadvance:
					char C = '\n';
					bool is_last_char = true;
					if(cursor_pos < line_length){
						C = toupper(LINE[cursor_pos]);
						is_last_char = false;
					}

					if(primary_state == NO_STATE){
						if(isblank(C))
							goto nextchar;
						if(is_last_char)
							goto nextline;
						if(isdigit(C))
							ERROR("Instruction cannot start with a number");
						if(!isprint(C))
							ERROR("Invalid ASCII character found");
						if(C == '@')
							ERROR("Etiquette reference cannot be used as an instruction");
						if(C == '#')
							goto nextline;
						primary_state = STATE_FETCHING_INITIAL_WORD;
						goto nextchar_noadvance;
					}else if(primary_state == STATE_FETCHING_INITIAL_WORD){
						if(isblank(C) || C == '#' || is_last_char){
							debug_cout("Loaded instruction " << current_word);
							instructions.add_new_instruction(current_word, std::string(LINE), current_line);
							current_word = "";
							primary_state = STATE_ARGUMENTS_SPACES;
							if(C == '#' || is_last_char)
								goto nextline;
							goto nextchar;
						}
						if(C == ':'){
							debug_cout("Etiquette " << current_word << " found");
							if(instructions.is_etiquette_defined(current_word))
								ERROR("Redefinition of etiquette "+current_word);
							instructions.etiquette_next_instruction(current_word);
							primary_state = STATE_EXPECT_EOL;
							goto nextchar;
						}
						if(!isalnum(C) && C != '_' && C != '-')
							ERROR("Invalid character found, can\'t be used as an instruction name or an etiquette");
						current_word += C;
					}else if(primary_state == STATE_FETCHING_ARGUMENTS){
						if(isblank(C) || C == '#' || is_last_char){
							if(current_word.length() > 0){
								debug_cout("  Added argument " << current_word);
								instructions.append_arg_to_last_instruction(current_word);
								current_word = "";
								primary_state = STATE_ARGUMENTS_SPACES;
							}
							if(C == '#' || is_last_char)
								goto nextline;
							goto nextchar;
						}
						if(!isalnum(C) && C != '@' && C != '_' && C != '-')
							ERROR("Invalid character found, can\'t be used as an argument or an etiquette reference");
						current_word += C;
					}else if(primary_state == STATE_ARGUMENTS_SPACES){
						if(!isblank(C)){
							primary_state = STATE_FETCHING_ARGUMENTS;
							goto nextchar_noadvance;
						}
					}else if(primary_state == STATE_EXPECT_EOL){
						if(C == '#' || is_last_char)
							goto nextline;
						if(!isblank(C))
							ERROR("Expected end-of-line");
					}

					nextchar:
					cursor_pos++;
				}
				nextline:
				current_line++;
			}
			#undef ERROR
			#define ERROR(STR) {errorize(op->raw_line, "COMPILATION ERROR: " STR, op->line_num, 0); return false;}

			// SECOND PASS [memwrite=0] - Determine instruction lengths (or execute third-pass if not referencing etiquettes)
			// THIRD PASS [memwrite=1] - Convert the instruction array to raw opcodes written into the memory
			uint32_t current_mem_ptr;
			debug_printf("Begining second pass");
			
			for(uint8_t memwrite = 0; memwrite <= 1; memwrite++){
				current_mem_ptr = 0;
				for(uint32_t i = 0; i < instructions.instructions.size(); i++){
					debug_printf("Processing instruction %u", i);
					asm_loader_raw_instruction* op = instructions.get_instruction(i);
					if(!op){
						debug_printf("Tried to process an instruction, but fetching resulted in a NULL!");
						return false;
					}

					vm_instruction opcode;
					memset(&opcode, 0, sizeof(opcode));
					if(opcode.OPCODE != 0 || opcode.oVAL != 0 || opcode.REG1 != 0)
						ERROR("Internal error, initial opcode not zero\'ed");

					opcode.OPCODE = get_instruction_opcode(op->INSTRUCTION_NAME);
					OPCODE_INSTRUCTION_FORMATS format = get_instruction_format(op->INSTRUCTION_NAME);

					if(op->written_to_mem && memwrite == 1){
						current_mem_ptr += vm_opcode_length[opcode.OPCODE];
						continue;
					}
					if(memwrite == 1)
						debug_printf("Pass 3 on line %s", op->raw_line.c_str());

					bool can_write = false;

					switch(format){
						case _OPCODE_INVALID_FORMAT_: ERROR("Unknown instruction name"); break;
						case OPCODE_REG_REG:
							if(op->INSTRUCTION_ARGS.size() != 2)
								ERROR("Invalid argument count (expected 2 register identifiers)");
							opcode.REG1 = get_register_code(op->INSTRUCTION_ARGS[0]);
							opcode.REG2 = get_register_code(op->INSTRUCTION_ARGS[1]);
							if(opcode.REG1 == _VM_INVALID_REG_)
								ERROR("Unknown register (first argument)");
							if(opcode.REG2 == _VM_INVALID_REG_)
								ERROR("Unknown register (second argument)");
							can_write = true;
						break;
						case OPCODE_REG:
							if(op->INSTRUCTION_ARGS.size() != 1)
								ERROR("Invalid argument count (expected 1 register identifier)");
							opcode.REG1 = get_register_code(op->INSTRUCTION_ARGS[0]);
							if(opcode.REG1 == _VM_INVALID_REG_)
								ERROR("Unknown register");
							can_write = true;
						break;
						case OPCODE_VALUE_32BIT:
							if(op->INSTRUCTION_ARGS.size() != 1)
								ERROR("Invalid argument count (expected 1 address or etiquette)");
							if(!argument_references_etiquette(op, 0) || memwrite == 1){
								if(!format_numeric_value(&instructions, op, 0, opcode.VAL))
									return false;
								can_write = true;
							}
						break;
						case OPCODE_REG_VALUE_32BIT:
							if(op->INSTRUCTION_ARGS.size() != 2)
								ERROR("Invalid argument count (expected a register identifier and an address, value or etiquette)");
							opcode.REG1 = get_register_code(op->INSTRUCTION_ARGS[0]);
							if(opcode.REG1 == _VM_INVALID_REG_)
								ERROR("Unknown register");

							if(!argument_references_etiquette(op, 1) || memwrite == 1){
								if(!format_numeric_value(&instructions, op, 1, opcode.oVAL))
									return false;
								can_write = true;
							}
						break;
						case OPCODE_VARIANT:
							if(op->INSTRUCTION_ARGS.size() != 1)
								ERROR("Invalid argument count (expected a variant register identifier, an address, value or etiquette)");
							opcode.REG2 = get_register_code(op->INSTRUCTION_ARGS[0]);
							if(opcode.REG2 == _VM_INVALID_REG_){
								if(!argument_references_etiquette(op, 0) || memwrite == 1){
									if(!format_numeric_value(&instructions, op, 0, opcode.oVAL))
										return false;
									can_write = true;
								}
							}else{
								can_write = true;
								opcode.OPTS |= VM_OPT_VARIANT_REG;
							}
						break;
						case OPCODE_REG_VARIANT:
							if(op->INSTRUCTION_ARGS.size() != 2)
								ERROR("Invalid argument count (expected a registry identifier and a variant register identifier, an address, value or etiquette)");
							opcode.REG1 = get_register_code(op->INSTRUCTION_ARGS[0]);
							if(opcode.REG1 == _VM_INVALID_REG_)
								ERROR("Unknown register");
							
							opcode.REG2 = get_register_code(op->INSTRUCTION_ARGS[1]);
							if(opcode.REG2 == _VM_INVALID_REG_){
								if(!argument_references_etiquette(op, 1) || memwrite == 1){
									if(!format_numeric_value(&instructions, op, 1, opcode.oVAL))
										return false;
									can_write = true;
								}
							}else{
								can_write = true;
								opcode.OPTS |= VM_OPT_VARIANT_REG;
							}
						break;
						case OPCODE_NO_ARGUMENTS: can_write = true; break;
						default:
							ERROR("Unknown format used by this instruction, cannot compile");
					}
					
					op->written_mem_addr = current_mem_ptr;
					if(can_write){
						//mem->write_address_ext(current_mem_ptr, (uint32_t*)&opcode, ROUND_UP(sizeof(opcode), 4));
						mem->write_address_ext(current_mem_ptr, (uint32_t*)&opcode, vm_opcode_length[opcode.OPCODE]);
						op->written_to_mem = true;
						debug_cout("Written " << op->raw_line << " at mem " << op->written_mem_addr);
					}else if(memwrite == 1){
						ERROR("Could not compile an instruction in third pass");
					}
					current_mem_ptr += vm_opcode_length[opcode.OPCODE];
				}
			}

			#if DEBUG_MODE
				debug_cout("COMPILATION SUMMARY");
				for(uint32_t i = 0; i < instructions.instructions.size(); i++){
					debug_cout("INSTR: " << instructions.instructions[i].raw_line << " @ " << instructions.instructions[i].written_mem_addr);
				}
				debug_cout("END OF SUMMARY");
			#endif

			return true;
			#undef ERROR
		}
		bool argument_references_etiquette(asm_loader_raw_instruction* op, uint16_t const& which_arg){
			return (op->INSTRUCTION_ARGS[which_arg][0] == '@');
		}

		bool format_numeric_value(asm_loader_instruction_store * store, asm_loader_raw_instruction * op, uint16_t const& which_arg, uint32_t & dst){
			if(op->INSTRUCTION_ARGS[which_arg][0] == '@'){
				debug_cout("Dereferencing " << op->INSTRUCTION_ARGS[which_arg]);
				std::string et = op->INSTRUCTION_ARGS[which_arg].substr(1);
				if(store->is_etiquette_defined(et)){
					asm_loader_raw_instruction* ref_op = store->get_etiquette_instruction(et);
					dst = ref_op->written_mem_addr;
					/*if(!ref_op->written_to_mem){
						errorize(op->raw_line, "ETIQUETTE SOLVER: Tried to reference an unwritten instruction (unknown addr)", op->line_num, 0);
						return false;
					}*/
					debug_cout("Resolved addr " << dst << " (" << store->get_etiquette_instruction(et)->raw_line << ")");
					return true;
				}else{
					errorize(op->raw_line, "ETIQUETTE SOLVER: Tried to use an undefined etiquette", op->line_num, 0);
					return false;
				}
			}

			dst = static_cast<uint32_t>(std::stoll(op->INSTRUCTION_ARGS[which_arg]));
			if(SSTR(dst) != op->INSTRUCTION_ARGS[which_arg]){
				errorize(op->raw_line, "Invalid formatting of an absolute value", op->line_num, 0);
				debug_cout("Had " << op->INSTRUCTION_ARGS[which_arg] << " coverted to " << SSTR(dst) << "(raw " << dst << ")");
				return false;
			}else{
				debug_cout("Had " << op->INSTRUCTION_ARGS[which_arg] << " coverted to " << SSTR(dst) << "(raw " << dst << ")");
			}
			return true;
		}
};