#define CONCAT(A,B) A B
#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))

#include <stdio.h>

#if DEBUG == 1
	#define debug_printf(str, ...) printf(CONCAT(str,"\n"),__VA_ARGS__)
	#define DEBUG_MODE 1
#else
	#define debug_printf(...) 0
	#define DEBUG_MODE 0
#endif