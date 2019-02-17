#include"main.h"

int main(void) {
	char teststr1[100] = " test stri p  ";
	char teststr2[100] = "  test stri p ";
	char* testres1 = strip(teststr1);
	char* testres2 = strip(teststr2);
	printf("%s\n", testres1);
	printf("%s\n", testres2);
    //free heaps
	free(testres1);
	free(testres2);
}
