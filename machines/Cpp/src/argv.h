#pragma once

#include "util.h"

std::string ARG_PROGRAM, ARG_DUMP_PATH;
uint32_t ARG_DUMP_OFFSET, ARG_DUMP_LENGTH;

bool process_arguments(int argc, char* argv[]){
    bool skipNext = false;
    for(int i = 1; i < argc; i++){
        if(skipNext){
            skipNext = false;
            continue;
        }

        std::string arg = std::string(argv[i]);
        if(arg == "-program"){
            if(i+1 < argc){
                ARG_PROGRAM = std::string(argv[i+1]);
                skipNext=true;
                continue;
            }else{
                printf("-program option requires a program path\n");
                return false;
            }
        }

        if(arg == "-memdump"){
            if(i+1 < argc){
                ARG_DUMP_PATH = std::string(argv[i+1]);
                skipNext=true;
                continue;
            }else{
                printf("-memdump option requires a dump path\n");
                return false;
            }
        }

        if(arg == "-dumpaddr"){
            if(i+1 < argc){
                ARG_DUMP_OFFSET = atoi(argv[i+1]);
                skipNext=true;
                continue;
            }else{
                printf("-dumpaddr option requires an address\n");
                return false;
            }
        }

        if(arg == "-dumplength"){
            if(i+1 < argc){
                ARG_DUMP_LENGTH = atoi(argv[i+1]);
                skipNext=true;
                continue;
            }else{
                printf("-dumplength option requires a value\n");
                return false;
            }
        }
    }

    return true;
}