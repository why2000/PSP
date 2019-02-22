//
//  errors.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include "errors.h"
extern FILE* read_fp;//"main.c"
extern RNL* rootRNL;//"main.c"
extern RNL* leaveRNL;//"main.c"
//报错系统
int errorfound(const int index) {//index为错误索引
    /**************************改动此函数时不要 删除 枚举类型中的任何项****************************/
    enum errornum { TMRIGHT = 1, TMLEFT, NEGSQRT, COMPRESULT, ZERODENO, ABNOMALSYN, TMSYN, NOINPUT, TMDOTS};
    switch (index) {
        case TMRIGHT:
            printf("右括号过多)))\n");
            break;
        case TMLEFT:
            printf("左括号过多(((\n");
            break;
        case NEGSQRT:
            printf("开非奇整数次方时底数小于零@@@\n");
            break;
        case COMPRESULT:
            printf("结果为虚数的幂运算^^^\n");
            break;
        case ZERODENO:
            printf("分母为零000\n");
            break;
        case ABNOMALSYN:
            printf("无法识别的输入$$$\n");
            printf("请检查运算式中是否混入字母或使用了中文括号\n");
            break;
        case TMSYN:
            printf("错误的运算式%%%%%%\n");
            break;
        case NOINPUT:
            printf("无输入???\n");
            break;
        case TMDOTS:
            printf("检测到多重小数点...\n");
            break;
    }
    
    return 2;
}
