//
//  reader.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

//记得在所有errorfound(0);之前free所有函数中创建的堆
//2.23晚:理论上实现了类型不一致检测（未测试）
//2.24晚:注意栈链表一定要保证前后各有一空位，否则会导致NULL指针在栈释放后被覆盖写入值，无法检测首尾
//2.25晚:正在实现单目运算符

#include"main.h"
ulong line_num = 0;
char token_name[MAX_TOKEN_SIZE];
//全局链表可以用NULL检测首尾
RNL* rootRNL;
RNL* leaveRNL;
FILE* read_fp;
FILE* write_fp;
int IF_STATUS = 0;
int JUDGE_STATUS = 0;

int main(int argc, char* argv[]) {
    int index;
    int* mainstatus = (int*)malloc(sizeof(int)*(argc));
    int failsum = 0;
    for (index = 1; index < argc; index++){
        write_fp = stdout;
        if(strcmp(argv[index], "-o") == 0){
            index++;
            write_fp = fopen(argv[index], "w");
            break;
        }
        mainstatus[index] = anastart(argv[index]);
        if (mainstatus[index] == 0){
            printf("Result: %s successful analyzed!\n", argv[index]);
        }else{
            failsum++;
            printf("Result: Failed to analyze %s!\n", argv[index]);
        }
    }
    printf("Analysis finished, %d succeeded, %d failed!\n", (argc-1-failsum), failsum);
    if(write_fp != stdout){
        fclose(write_fp);
    }
    free(mainstatus);
    mainstatus = NULL;
}

int anastart(const char* filename){
    //0为成功
    int ana_status = 1;
    line_num = 0;
    read_fp = NULL;
//    int curstatus;
    if((read_fp = fopen(filename, "r")) == NULL){
        printf("Error! Failed to open %s.\n", filename);
        return 1;
    }
    rootRNL = (RNL*)malloc(sizeof(RNL));
    rootRNL->FPL = NULL;
    rootRNL->next = (RNL*)malloc(sizeof(RNL));
    rootRNL->next->value_type = EMPTY_TOKEN;
    rootRNL->next->var_type = STARTOFSTA;
    rootRNL->next->last = rootRNL;
    rootRNL->last = (RNL*)malloc(sizeof(RNL));
    rootRNL->last->value_type = EMPTY_TOKEN;
    rootRNL->last->var_type = ENDOFSTA;
    rootRNL->last->next = rootRNL;
    rootRNL->value_type = EMPTY_TOKEN;
    rootRNL->var_type = ENDOFSTA;
    leaveRNL = rootRNL;
    fprintf(write_fp, "文件\"%s\"语法解析结果:\n", filename);
    EDL* EDL_cur = ExtDefList();
    if (EDL_cur){
        EDLoutput(EDL_cur);
        ana_status = 0;
    }else{
        ana_status = 1;
    }
    fclose(read_fp);
    return ana_status;
}










