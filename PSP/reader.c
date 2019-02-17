//
//  reader.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/17.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include "main.h"
int readinput(char *input, char *buf) {//input进行后续计算,buf用于格式化输出
    int i = 0,j=0;
    int minus = 0;
    
    while (((input[i] = fgetc(stdin))) != '\n')
    {
        buf[j] = input[i];
        if (input[i] == '-') {//非常神秘的没有验证正确性的自己脑子一抽想出来的负数的暴力处理办法
            if (i == 0) {//负号在首位时直接添0
                input[i++] = '0';
                input[i] = '-';
            }
            else if (input[i - 1] == '(') {//负号在左括号后时直接添0
                /*******这个不能和上面的合并，否则读取input[-1]会导致内存错误******/
                input[i++] = '0';
                input[i] = '-';
            }
            
            else{//负号在数字的后面时添置括号
                input[i++] = '+';
                input[i++] = '(';
                input[i++] = '0';
                input[i] = '-';
                ++minus;
            }
        }
        if (input[i] == '+') {//顺便把正号一起处理了
            if (i == 0) {//首位添0
                input[i++] = '0';
                input[i] = '+';
            }
            else if (input[i - 1] == '(') {//左括号后添0
                input[i++] = '0';
                input[i] = '+';
            }    
        }if (input[i] == '.'&&!(input[i-1]>='0'&&input[i-1]<='9')) {//当然还有小数点
            input[i++] = '0';
            input[i] = '.';//任何情况下没接在数字后面的小数点只要直接变成0.就行了
            
        }
        if (input[i] == ' ')
        {
            i--;
            j--;
        }
        i++;
        j++;
    }
    buf[j] = '\0';
    for (int k = 1; k <= minus; k++) {//补全由于处理负数产生的括号
        input[i++] = ')';
    }
    //**********************错误8：无输入******************************
    if (input[0]=='\n') {
        return errorfound(8);
    }
    //************************ERROR8*********************************
    
    input[i] = '\0';
    
    
    //特殊命令：
    //1.exit（或quit）退出程序
    //2.help 打开帮助界面
    //3.change 修改精度
    if (strcmp(input, "quit")==0 || strcmp(input, "exit")==0) {
        strcpy(input, "");
        return 0;
    }
    else if (strcmp(input, "help") == 0) {
        outputhelp();
        return 2;
    }
    else if (strcmp(input, "change") == 0) {
        preciquest();
        return 2;
    }
    else {
        return 1;
    }
    
    
}
