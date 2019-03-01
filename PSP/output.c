//
//  output.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include "output.h"
extern FILE* read_fp;//"main.c"
extern FILE* write_fp;//"main.c"
extern FILE* formatter_fp;
extern RNL* rootRNL;//"main.c"
extern RNL* leaveRNL;//"main.c"
int tab_number = 1;
int formatter_tab = 0;

void tab_print(){
    int i;
    for(i = 0; i < tab_number; i++){
        fprintf(write_fp, "    ");
    }
}
void formatter_print(){
    int i;
    for(i = 0; i < formatter_tab; i++){
        fprintf(formatter_fp, "    ");
    }
}

void PREoutput(PRECOMPILES* curPC){
    while(curPC->next != NULL){
        tab_print();
        fprintf(write_fp, "预编译指令位置: 第%d行\n", curPC->line);
        tab_number += 1;
        tab_print();
        fprintf(write_fp, "指令内容:\n");
        tab_number += 1;
        if(curPC->PC_type == INCLUDE){
            tab_print();
            fprintf(write_fp, "INCLUDE: 包含%s\n", curPC->firstname);
            fprintf(formatter_fp, "#include<%s>\n", curPC->firstname);
        }
        else{
            tab_print();
            fprintf(write_fp, "DEFINE: 将符号%s替换为%s\n", curPC->firstname, curPC->secondname);
            fprintf(formatter_fp, "#define %s %s\n", curPC->firstname, curPC->secondname);
        }
        tab_number -= 1;
        tab_number -= 1;
        curPC = curPC->next;
    }
}


void ANNoutput(ANNOTATIONS* curANN){
    while(curANN->next != NULL){
        tab_print();
        fprintf(write_fp, "注释位置: 第%d行\n", curANN->line);
        tab_number += 1;
        tab_print();
        fprintf(write_fp, "注释内容:\n");
        tab_number -= 1;
        int buf_tab_num = tab_number;
        if(curANN->ANN_type == SINGLEANN){
            tab_number = 0;
            tab_print();
            fprintf(write_fp, "//%s\n", curANN->ANN_string);
            tab_number = buf_tab_num;
        }
        else{
            tab_number = 0;
            tab_print();
            fprintf(write_fp, "/*%s*/\n", curANN->ANN_string);
            tab_number = buf_tab_num;
        }
        curANN = curANN->next;
    }
}


void EDLoutput(EDL* curEDL){
    while(curEDL->ED_kind != ENDOFSTA){
        EDL* buf_EDL = curEDL;
        if(curEDL->ED_kind == EXTVARDEF){
            tab_print();
            fprintf(write_fp, "外部变量定义:\n");
            tab_number += 1;
            EVDoutput(curEDL->ED->EVD);
            fprintf(formatter_fp, ";\n");
            free(curEDL->ED);
            curEDL->ED->EVD = NULL;
            tab_number -= 1;
        }
        else if(curEDL->ED_kind == FUNCDEF){
            tab_print();
            fprintf(write_fp, "函数定义:\n");
            tab_number += 1;
            FUNCDoutput(curEDL->ED->FUNCD);
            free(curEDL->ED);
            tab_number -= 1;
        }
        curEDL = curEDL->EDL;
        free(buf_EDL);
    }
    fprintf(write_fp, "解析成功!\n");
    return;
}


void EVDoutput(EVD* curEVD){
    char buf_string[MAX_TOKEN_SIZE];
    to_string(curEVD->EVD_kind, buf_string) ;
    tab_print();
    fprintf(write_fp, "类型: %s\n", buf_string);
    formatter_print();
    fprintf(formatter_fp, "%s ", buf_string);
    tab_print();
    fprintf(write_fp, "变量序列: \n");
    tab_number += 1;
    EVNLoutput(curEVD->EVNL);
    tab_number -= 1;
//    free(curEVD);
    return;
}

void EVNLoutput(EVNL* curEVNL){
    if(strcmp(curEVNL->var_name, "###") == 0){
//        free(curEVNL);
        return;
    }
    tab_print();
    fprintf(write_fp, "变量名: %s\n", curEVNL->var_name);
    if(strcmp(curEVNL->next->var_name, "###") == 0){
        fprintf(formatter_fp, "%s", curEVNL->var_name);
    }else{
        fprintf(formatter_fp, "%s, ", curEVNL->var_name);
    }
    EVNLoutput(curEVNL->next);
//    free(curEVNL);
    curEVNL = NULL;
    return;
}

void FUNCDoutput(FUNCD* curFUNCD){
    char buf_string[MAX_TOKEN_SIZE];
    to_string(curFUNCD->FUNC_kind, buf_string);
    tab_print();
    fprintf(write_fp, "函数类型: %s\n", buf_string);
    fprintf(formatter_fp, "%s ", buf_string);
    tab_print();
    fprintf(write_fp, "函数名: %s\n", curFUNCD->FUNC_name);
    fprintf(formatter_fp, "%s(", curFUNCD->FUNC_name);
    tab_print();
    fprintf(write_fp, "函数形参:\n");
    tab_number += 1;
    FPLoutput(curFUNCD->FPL);
    tab_number -= 1;
    tab_print();
    fprintf(formatter_fp, "){\n");
    fprintf(write_fp, "复合语句:\n");
    tab_number += 1;
    COMPSoutput(curFUNCD->FUNCB);
//    free(curFUNCD);
    tab_number -= 1;
    fprintf(formatter_fp, "}\n");
    return;
}

void FPLoutput(FPL* curFPL){
    if(curFPL->FP_kind == EMPTY_TOKEN)return;
    char buf_string[MAX_TOKEN_SIZE];
    to_string(curFPL->FP_kind, buf_string);
    tab_print();
    fprintf(write_fp, "参数类型: %s, 参数名: %s\n", buf_string, curFPL->FP_name);
    if(curFPL->next->FP_kind == EMPTY_TOKEN){
        fprintf(formatter_fp, "%s %s", buf_string, curFPL->FP_name);
    }else{
        fprintf(formatter_fp, "%s %s, ", buf_string, curFPL->FP_name);
    }
    FPLoutput(curFPL->next);
    free(curFPL);
    return;
}

void COMPSoutput(COMPS* curCOMPS){
    formatter_tab += 1;
    while(curCOMPS->COMP_kind != ENDOFSTA){
        COMPS* buf_COMPS = curCOMPS;
        if(curCOMPS->COMP_kind == EXTVARDEF){
            tab_print();
            fprintf(write_fp, "复合语句的变量定义:\n");
            tab_number += 1;
            EVDoutput(curCOMPS->COMP->EVD);
            fprintf(formatter_fp, ";\n");
//            free(curCOMPS->COMP);
            tab_number -= 1;
        }
        else if(curCOMPS->COMP_kind == RETURNSTA){
            tab_print();
            fprintf(write_fp, "返回语句:\n");
            formatter_print();
            fprintf(formatter_fp, "return ");
            tab_number += 1;
            NMSoutput(curCOMPS->COMP->STA->RTS->NMS);
            fprintf(formatter_fp, ";\n");
//            free(curCOMPS->COMP->STA->RTS);
//            free(curCOMPS->COMP->STA);
//            free(curCOMPS->COMP);
            tab_number -= 1;
        }
        else if(curCOMPS->COMP_kind == IFSTA){
            if(curCOMPS->next->COMP_kind == ELSESTA){
                tab_print();
                fprintf(write_fp, "IF-ELSE语句:\n");
                tab_number += 1;
                tab_print();
                formatter_print();
                fprintf(formatter_fp, "if(");
                fprintf(write_fp, "条件表达式:\n");
                tab_number += 1;
                NMSoutput(curCOMPS->COMP->STA->IFTH->NMS);
                tab_number -= 1;
                fprintf(formatter_fp, "){\n");
                tab_print();
                fprintf(write_fp, "IF子句:\n");
                tab_number += 1;
                COMPSoutput(curCOMPS->COMP->STA->IFTH->IFCOMPS);
                tab_number -= 1;
                formatter_print();
                fprintf(formatter_fp, "}\n");
                //注意这里少一个-1留给else
//                free(curCOMPS->COMP->STA->IFTH);
//                free(curCOMPS->COMP->STA);
//                free(curCOMPS->COMP);
            }else{
                tab_print();
                fprintf(write_fp, "IF-THEN语句:\n");
                formatter_print();
                fprintf(formatter_fp, "if(");
                tab_number += 1;
                tab_print();
                fprintf(write_fp, "条件表达式:\n");
                tab_number += 1;
                NMSoutput(curCOMPS->COMP->STA->IFTH->NMS);
                fprintf(formatter_fp, "){\n");
                tab_number -= 1;
                tab_print();
                fprintf(write_fp, "IF子句:\n");
                tab_number += 1;
                COMPSoutput(curCOMPS->COMP->STA->IFTH->IFCOMPS);
                formatter_print();
                fprintf(formatter_fp, "}\n");
                tab_number -= 1;
                tab_number -= 1;
//                free(curCOMPS->COMP->STA->IFTH);
//                free(curCOMPS->COMP->STA);
//                free(curCOMPS->COMP);
            }
        }
        else if(curCOMPS->COMP_kind == ELSESTA){
            tab_print();
            fprintf(write_fp, "ELSE子句:\n");
            formatter_print();
            fprintf(formatter_fp, "else{\n");
            tab_number += 1;
            COMPSoutput(curCOMPS->COMP->STA->ELTH->ELSECOMPS);
            formatter_print();
            fprintf(formatter_fp, "}\n");
            tab_number -= 1;
            tab_number -= 1;
//            free(curCOMPS->COMP->STA->ELTH);
//            free(curCOMPS->COMP->STA);
//            free(curCOMPS->COMP);
        }
        else if(curCOMPS->COMP_kind == WHILESTA){
            tab_print();
            fprintf(write_fp, "WHILE语句:\n");
            formatter_print(); 
            fprintf(formatter_fp, "while(");
            tab_number += 1;
            tab_print();
            fprintf(write_fp, "条件表达式:\n");
            tab_number += 1;
            NMSoutput(curCOMPS->COMP->STA->WHILETH->NMS);
            fprintf(formatter_fp, "){\n");
            tab_number -= 1;
            tab_print();
            fprintf(write_fp, "WHILE循环体:\n");
            tab_number += 1;
            COMPSoutput(curCOMPS->COMP->STA->WHILETH->WHILECOMPS);
            formatter_print();
            fprintf(formatter_fp, "}\n");
            tab_number -= 1;
            tab_number -= 1;
//            free(curCOMPS->COMP->STA->WHILETH);
//            free(curCOMPS->COMP->STA);
//            free(curCOMPS->COMP);
        }
        else if(curCOMPS->COMP_kind == BREAKSTA){
            tab_print();
            fprintf(write_fp, "BREAK语句\n");
            formatter_print();
            fprintf(formatter_fp, "break;\n");
        }
        else if(curCOMPS->COMP_kind == CONTINUESTA){
            tab_print();
            fprintf(write_fp, "CONTINUE语句\n");
            formatter_print();
            fprintf(formatter_fp, "continue;\n");
        }
        else if(curCOMPS->COMP_kind == FORSTA){
            tab_print();
            fprintf(write_fp, "FOR语句:\n");
            formatter_print();
            fprintf(formatter_fp, "for(");
            tab_number += 1;
            tab_print();
            fprintf(write_fp, "循环初始化表达式:\n");
            tab_number += 1;
            NMSoutput(curCOMPS->COMP->STA->FORTH->NMS_start);
            fprintf(formatter_fp, "; ");
            tab_number -= 1;
            tab_print();
            fprintf(write_fp, "条件表达式:\n");
            tab_number += 1;
            NMSoutput(curCOMPS->COMP->STA->FORTH->NMS_check);
            fprintf(formatter_fp, "; ");
            tab_number -= 1;
            tab_print();
            fprintf(write_fp, "循环增量表达式:\n");
            tab_number += 1;
            NMSoutput(curCOMPS->COMP->STA->FORTH->NMS_end);
            fprintf(formatter_fp, "){\n");
            tab_number -= 1;
            tab_print();
            fprintf(write_fp, "FOR循环体:\n");
            tab_number += 1;
            COMPSoutput(curCOMPS->COMP->STA->FORTH->FORCOMPS);
            formatter_print();
            fprintf(formatter_fp, "}\n");
            tab_number -= 1;
            tab_number -= 1;
//            free(curCOMPS->COMP->STA->FORTH);
//            free(curCOMPS->COMP->STA);
//            free(curCOMPS->COMP);
        }
        else if(curCOMPS->COMP_kind == NORMSTA){
            tab_print();
            fprintf(write_fp, "表达式语句:\n");
            tab_number += 1;
            formatter_print();
            NMSoutput(curCOMPS->COMP->STA->NMS);
            fprintf(formatter_fp, ";\n");
            tab_number -= 1;
//            free(curCOMPS->COMP->STA);
//            free(curCOMPS->COMP);
        }
        curCOMPS = buf_COMPS->next;
        //free的上个curCOMPS
//        free(buf_COMPS);
    }
    formatter_tab -= 1;
    return;
}



void NMSoutput(NMS* curNMS){
    char buf_string[MAX_TOKEN_SIZE];
    //检查是否打印为变量，否则打印表达式
    if(curNMS->NM_kind == IDENT){
        tab_print();
        fprintf(write_fp, "变量:\n");
        tab_number += 1;
        to_string(curNMS->res_kind, buf_string);
        tab_print();
        fprintf(write_fp, "变量类型: %s\n", buf_string);
        tab_print();
        fprintf(write_fp, "变量名: %s\n", curNMS->NMS_name);
        fprintf(formatter_fp, "%s", curNMS->NMS_name);
        tab_number -= 1;
    }
    else if(curNMS->NM_kind == FUNC){
        tab_print();
        fprintf(write_fp, "函数调用:\n");
        tab_number += 1;
        to_string(curNMS->res_kind, buf_string);
        tab_print();
        fprintf(write_fp, "函数类型: %s\n", buf_string);
        tab_print();
        fprintf(write_fp, "函数名: %s\n", curNMS->NMS_name);
        fprintf(formatter_fp, "%s", curNMS->NMS_name);
        tab_print();
        fprintf(write_fp, "函数实参:\n");
        fprintf(formatter_fp, "(");
        tab_number += 1;
        APLoutput(curNMS->APL);
        tab_number -= 1;
        fprintf(formatter_fp, ")");
        tab_number -= 1;
    }
    else if(check_const(curNMS->NM_kind)){
        tab_print();
        fprintf(write_fp, "常量:\n");
        tab_number += 1;
        to_string(curNMS->NM_kind, buf_string);
        tab_print();
        fprintf(write_fp, "常量类型: %s\n", buf_string);
        tab_print();
        fprintf(write_fp, "常量值: %s\n", curNMS->NMS_name);
        fprintf(formatter_fp, "%s", curNMS->NMS_name);
        tab_number -= 1;
    }
    else{
        tab_print();
        fprintf(write_fp, "表达式:\n");
        tab_number += 1;
        to_string(curNMS->res_kind, buf_string);
        tab_print();
        fprintf(write_fp, "结果类型: %s\n", buf_string);
        tab_print();
        to_string(curNMS->NM_kind, buf_string);
        fprintf(write_fp, "运算符: %s\n", buf_string);
        if(curNMS->NM_kind == RDBPLUS || curNMS->NM_kind == RDBMINUS){
            tab_number += 1;
            tab_print();
            fprintf(write_fp, "左操作符: \n");
            if(curNMS->NM_kind == RDBPLUS){
                tab_number += 1;
                NMSoutput(curNMS->left);
                fprintf(formatter_fp, "++");
            }
            else if(curNMS->NM_kind == RDBMINUS){
                tab_number += 1;
                NMSoutput(curNMS->left);
                fprintf(formatter_fp, "--");
            }
            else{
                fprintf(formatter_fp, "(");
                tab_number += 1;
                NMSoutput(curNMS->left);
                fprintf(formatter_fp, ")%s", buf_string);
            }
            tab_number -= 1;
            tab_number -= 1;
        }
        else if(curNMS->NM_kind == LDBPLUS || curNMS->NM_kind == LDBMINUS || curNMS->NM_kind == NOT || curNMS->NM_kind == REVERSE){
            tab_number += 1;
            tab_print();
            fprintf(write_fp, "右操作符: \n");
            if(curNMS->NM_kind == RDBPLUS){
                fprintf(formatter_fp, "++");
                tab_number += 1;
                NMSoutput(curNMS->right);
                tab_number -= 1;
            }
            else if(curNMS->NM_kind == RDBMINUS){
                fprintf(formatter_fp, "--");
                tab_number += 1;
                NMSoutput(curNMS->right);
                tab_number -= 1;
            }
            else{
                fprintf(formatter_fp, "%s(", buf_string);
                tab_number += 1;
                NMSoutput(curNMS->right);
                fprintf(formatter_fp, ")");
                tab_number -= 1;
            }
            tab_number -= 1;
        }
        else if(order_operator(curNMS->NM_kind) == 14){
            tab_number += 1;
            tab_print();
            fprintf(write_fp, "左操作符: \n");
            tab_number += 1;
            NMSoutput(curNMS->left);
            tab_number -= 1;
            fprintf(formatter_fp, " %s ", buf_string);
            tab_print();
            fprintf(write_fp, "右操作符: \n");
            tab_number += 1;
            NMSoutput(curNMS->right);
            tab_number -= 1;
            tab_number -= 1;
        }
        else{
            tab_number += 1;
            tab_print();
            fprintf(write_fp, "左操作符: \n");
            if(check_const(curNMS->left->NM_kind) || curNMS->left->NM_kind == IDENT){
                tab_number += 1;
                NMSoutput(curNMS->left);
                tab_number -= 1;
            }
            else{
                fprintf(formatter_fp, "(");
                tab_number += 1;
                NMSoutput(curNMS->left);
                tab_number -= 1;
                fprintf(formatter_fp, ")");
            }
            fprintf(formatter_fp, " %s ", buf_string);
            tab_print();
            fprintf(write_fp, "右操作符: \n");
            if(check_const(curNMS->right->NM_kind) || curNMS->right->NM_kind == IDENT){
                tab_number += 1;
                NMSoutput(curNMS->right);
                tab_number -= 1;
            }
            else{
                fprintf(formatter_fp, "(");
                tab_number += 1;
                NMSoutput(curNMS->right);
                tab_number -= 1;
                fprintf(formatter_fp, ")");
            }
            tab_number -= 1;
        }
        tab_number -= 1;
    }
//    free(curNMS);
    return;
}


void APLoutput(APL* curAPL){
    if(curAPL->NMS->res_kind == EMPTY_TOKEN)return;
    char buf_string[MAX_TOKEN_SIZE];
    to_string(curAPL->NMS->res_kind, buf_string);
    tab_print();
    fprintf(write_fp, "实参类型: %s, 实参表达式:\n", buf_string);
    tab_number += 1;
    NMSoutput(curAPL->NMS);
    if(curAPL->next->NMS->res_kind == EMPTY_TOKEN){
        ;
    }
    else{
        fprintf(formatter_fp, ", ");
    }
    tab_number -= 1;
    APLoutput(curAPL->next);
//    free(curAPL);
    return;
}









