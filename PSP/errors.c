//
//  errors.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include "errors.h"
extern FILE* read_fp;//"main.c"
extern FILE* write_fp;//"main.c"
extern RNL* rootRNL;//"main.c"
extern RNL* leaveRNL;//"main.c"
extern int line_num;//"main.c"

//报错系统
void* errorfound(enum errortype index) {//index为错误索引
    /**************************改动此函数时不要 删除 枚举类型中的任何项****************************/
    printf("ERROR!!!\n");
    printf("Line %d:", line_num);
    switch (index) {
        case SYNERR:
            fprintf(write_fp, "语法错误:\n");
            fprintf(write_fp, "无法解析的符号!\n");
        case TMRIGHT:
            fprintf(write_fp, "表达式解析失败:\n");
            fprintf(write_fp, "右括号过多!\n");
            break;
        case TMLEFT:
            fprintf(write_fp, "表达式解析失败:\n");
            fprintf(write_fp, "左括号过多!\n");
            break;
        case UNREGISTED:
            fprintf(write_fp, "符号获取失败:\n");
            fprintf(write_fp, "符号未声明或参数列表与原始声明不一致!\n");
            break;
        case REDUNDANT:
            fprintf(write_fp, "声明失败:\n");
            fprintf(write_fp, "相同代码块内存在重复声明!\n");
            break;
        case INVALID:
            fprintf(write_fp, "程序错误:\n");
            fprintf(write_fp, "检测到无效类型!\n");
            break;
        case ABNOMALSYN:
            fprintf(write_fp, "无法识别的输入$$$\n");
            fprintf(write_fp, "请检查运算式中是否混入字母或使用了中文括号\n");
            break;
        case TMSYN:
            fprintf(write_fp, "错误的运算式%%%%%%\n");
            break;
        case NOINPUT:
            fprintf(write_fp, "无输入???\n");
            break;
        case TMDOTS:
            fprintf(write_fp, "检测到多重小数点...\n");
            break;
    }
    fprintf(write_fp, "解析失败！\n");
    return NULL;
}
