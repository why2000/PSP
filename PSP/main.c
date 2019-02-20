//
//  reader.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include"main.h"
ulong line_num = 0;
char token_name[MAX_TOKEN_SIZE];
int main(int argc, char* argv[]) {
    int index;
    int* mainstatus = (int*)malloc(sizeof(int)*(argc));
    for (index = 1; index < argc; index++){
        mainstatus[index] = anastart(argv[index]);
        if (mainstatus[index] == 0){
            printf("Result: %s successful analyzed!\n", argv[index]);
        }else{
            printf("Result: Failed to analyze %s!\n", argv[index]);
        }
    }
    //utils test
//    char teststr1[100] = " test stri p  ";
//    char teststr2[100] = "  test stri p ";
//    char* testres1 = strip(teststr1);
//    char* testres2 = strip(teststr2);
//    printf("%s\n", testres1);
//    printf("%s\n", testres2);
    //result output
    int failsum = 0;
    for(index = 1; index < argc; index++){
        if(mainstatus[index] != 0){
            failsum++;
        }
    }
    printf("Analysis finished, %d succeeded, %d failed!\n", (argc-1-failsum), failsum);
    //free heaps
    free(mainstatus);
//    free(testres1);
//    free(testres2);
}

int anastart(const char* filename){
    FILE* read_fp = NULL;
//    int curstatus;
    if((read_fp = fopen(filename, "r")) == NULL){
        printf("Error! Failed to open %s.\n", filename);
        return 1;
    }
    EDL* curEDL;
    curEDL = ExtDefList(read_fp);
    fclose(read_fp);
    if (curEDL){
        EDLoutput(curEDL, NULL);
        free(curEDL);
        fclose(read_fp);
        return 0;
    }else{
        fclose(read_fp);
        return 1;
    }
    return 0;
}










