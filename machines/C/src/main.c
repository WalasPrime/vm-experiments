#include <stdio.h>

int main(){
	printf("Hello world!\n");
	#if DEBUG
		printf("DEBUG MODE!");
	#endif
}