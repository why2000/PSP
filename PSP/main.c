//
//  reader.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/17.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include"main.h"

int main(int argc, char* argv[]) {
    int index;
    for (index = 1; index < argc; index++){
        printf("%s", argv[index]);
    }
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
