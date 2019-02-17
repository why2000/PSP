#include<main.h>


//remove left spaces, return in heap
char* lstrip(const char* oristr) {
	//heap
	char* newstr = (char*)malloc(strlen(oristr)+1);
	char* orip;
	char* newp = newstr;
	for (orip = oristr; *orip == ' '; orip++);
	for (; *orip != '\0'; orip++, newp++) {
		*newp = *orip;
	}
	*newp = '\0';
	return newstr;
}

//remove right spaces, return in heap
char* rstrip(const char* oristr) {
	//heap
	char* newstr = (char*)malloc(strlen(oristr)+1);
	int orip;
	int newp;
	int length = strlen(oristr);
	for (orip = length - 1, newp = length - 1; oristr[orip] == ' '; orip--, newp--);
	newstr[newp + 1] = '\0';
	for (; orip >= 0; orip--, newp--) {
		newstr[newp] = oristr[orip];
	}
	return newstr;
}

//remove spaces, return in heap
char* strip(const char* oristr) {
	//heap
	char* newstr = (char*)malloc(strlen(oristr)+1);
	//heap
	char* lbufstr = (char*)malloc(strlen(oristr)+1);
	lbufstr = lstrip(oristr);
	newstr = rstrip(lbufstr);
	free(lbufstr);
	return newstr;
}
