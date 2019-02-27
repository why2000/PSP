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
extern int ERROR_STATUS;
extern int* fpipe;

//报错系统
void* errorfound(enum errortype index) {//index为错误索引
    /**************************改动此函数时不要 删除 枚举类型中的任何项****************************/
    fprintf(write_fp, "ERROR!!!\n");
    fprintf(write_fp, "Line %d:", line_num);
    ERROR_STATUS = 1;
    switch (index) {
        case SYNERR:
            fprintf(write_fp, "语法错误:\n");
            fprintf(write_fp, "无法解析的符号!\n");
            break;
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
            fprintf(write_fp, "语法错误:\n");
            fprintf(write_fp, "检测到无效类型!\n");
            break;
        case DIFFOP:
            fprintf(write_fp, "表达式解析失败:\n");
            fprintf(write_fp, "操作数类型错误!\n");
            break;
        case ASSIGNERR:
            fprintf(write_fp, "表达式解析失败:\n");
            fprintf(write_fp, "左值操作符作用于非左值表达式!\n");
            break;
        case ELSEONLY:
            fprintf(write_fp, "语法错误:\n");
            fprintf(write_fp, "单独存在的ELSE语句\n");
            break;
        case RETERR:
            fprintf(write_fp, "表达式解析失败:\n");
            fprintf(write_fp, "函数返回值类型错误!\n");
            break;
        case UNSUPPORTEDYET:
            fprintf(write_fp, "表达式解析失败:\n");
            fprintf(write_fp, "暂不支持的操作符!\n");
            break;
        case ERRONESIDE:
            fprintf(write_fp, "表达式解析失败:\n");
            fprintf(write_fp, "单目运算符仅有一侧均可取值!\n");
            break;
            
    }
    close(fpipe[0]);
    char child[BUFFER_SIZE] = "1";
    write(fpipe[1], child, strlen(child) + 1); 
    sleep(2);
    fclose(read_fp);
    fflush(write_fp);
    exit(0);    
    return NULL;
}
