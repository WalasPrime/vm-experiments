#pragma once
#include <fstream>

#include "util.h"

class asm_loader {
	private:
	public:
		asm_loader(){

		}

		void parse_file(std::string path){
			std::ifstream f(path);
			if(!f.is_open()){
				debug_cout("File " << path << " could not be opened for compiling");
				return;
			}
			debug_cout("Opened " << path << " for compilation");

			char LINE[512];
			while(!f.eof()){
				f.getline(LINE, 512);

				uint16_t cursor_pos = 0;
				uint16_t line_length = strlen(LINE);

				debug_cout("LOADED LINE: " << std::string(LINE) << " (length " << line_length << ")");

				while(cursor_pos < line_length){
					
					cursor_pos++;
				}
			}
		}
};