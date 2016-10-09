#pragma once
#define CONCAT(A,B) A B
#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))

#include <stdio.h>
#include <string>
#include <iostream>

#if DEBUG == 1
	#define debug_cout(A) std::cout << A << std::endl
	#define DEBUG_MODE 1
#else
	#define debug_printf(...) 0
	#define debug_cout(A) 0
	#define DEBUG_MODE 0
#endif

#ifndef _MSC_VER
	/**
	 * C++ version 0.4 char* style "itoa":
	 * Written by Lukás Chmela
	 * Released under GPLv3.

	 */
	char* itoa(int value, char* result, int base) {
		// check that the base if valid
		if (base < 2 || base > 36) { *result = '\0'; return result; }

		char* ptr = result, *ptr1 = result, tmp_char;
		int tmp_value;

		do {
			tmp_value = value;
			value /= base;
			*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
		} while ( value );

		// Apply negative sign
		if (tmp_value < 0) *ptr++ = '-';
		*ptr-- = '\0';
		while(ptr1 < ptr) {
			tmp_char = *ptr;
			*ptr--= *ptr1;
			*ptr1++ = tmp_char;
		}
		return result;
	}

	#if DEBUG_MODE
		#define debug_printf(str, ...) printf(CONCAT(str,"\n"),##__VA_ARGS__)
	#endif
#else
	#if DEBUG_MODE
		#define debug_printf(str, ...) printf(CONCAT(str,"\n"),__VA_ARGS__)
	#endif
#endif