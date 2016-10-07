#pragma once
#include <fstream>
#include <ctype.h>
#include <cstring>
#include <vector>
#include <map>

#include "util.h"

class asm_loader_raw_instruction {
	public:
		std::string INSTRUCTION_NAME;
		std::vector<std::string> INSTRUCTION_ARGS;

		asm_loader_raw_instruction(std::string const& name) : INSTRUCTION_NAME(name) {}

		void add_arg(std::string const& arg){
			INSTRUCTION_ARGS.push_back(arg);
		}
};

class asm_loader_instruction_store {
	public:
		std::vector<asm_loader_raw_instruction> instructions;
		std::map<std::string, uint32_t> etiquettes;

		void add_new_instruction(std::string const& name){
			instructions.push_back(asm_loader_raw_instruction(name));
		}

		void etiquette_next_instruction(std::string const& name){
			etiquettes[name] = instructions.size();
		}

		bool is_etiquette_defined(std::string const& name){
			return (etiquettes.find(name) != etiquettes.end());
		}

		void append_arg_to_last_instruction(std::string const& arg){
			instructions[instructions.size()-1].add_arg(arg);
		}
};

class asm_loader {
	private:
		enum PARSER_STATES {
			NO_STATE,

			STATE_FETCHING_INITIAL_WORD,	// Reading some characters for the first word
			STATE_EXPECT_EOL,				// Excepting \n at the next character
			STATE_FETCHING_ARGUMENTS		// Reading characters for arguments
		};

		PARSER_STATES primary_state;
	public:
		std::string last_error_str;
		uint16_t last_error_line;
		uint16_t last_error_col;
		std::string last_error_line_content;

		asm_loader(){
			primary_state = NO_STATE;
			last_error_str = "";
			last_error_line_content = "";
			last_error_line = 0;
			last_error_col = 0;
		}

		void errorize(std::string raw_line, std::string description, uint16_t line_num, uint16_t column){
			last_error_str = description;
			last_error_line = line_num;
			last_error_col = column;
			last_error_line_content = raw_line;
		}

		bool parse_file(std::string path){
			std::ifstream f(path.c_str());
			if(!f.is_open()){
				debug_cout("File " << path << " could not be opened for compiling");
				return false;
			}
			debug_cout("Opened " << path << " for compilation");

			asm_loader_instruction_store instructions;
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
				while(cursor_pos < line_length){
					nextchar_noadvance:
					char C = toupper(LINE[cursor_pos]);

					if(primary_state == NO_STATE){
						if(isblank(C))
							goto nextchar;
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
						if(isblank(C) || C == '#'){
							debug_cout("Loaded instruction " << current_word);
							instructions.add_new_instruction(current_word);
							current_word = "";
							primary_state = STATE_FETCHING_ARGUMENTS;
							if(C == '#')
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
						if(isblank(C) || C == '#'){
							if(current_word.length() > 0)
								instructions.append_arg_to_last_instruction(current_word);
							if(C == '#')
								goto nextline;
							goto nextchar;
						}
						if(!isalnum(C) && C != '@' && C != '_' && C != '-')
							ERROR("Invalid character found, can\'t be used as an argument or an etiquette reference");
						current_word += C;
					}else if(primary_state == STATE_EXPECT_EOL){
						if(C == '#')
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

			return true;
		}
};