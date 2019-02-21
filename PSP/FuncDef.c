//
//  FuncDef.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/21.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include "FuncDef.h"
extern int line_num;//"main.c"
extern char token_name[MAX_TOKEN_SIZE];//"main.c"

FUNCD* FuncDef(FILE* read_fp, enum token_kind declare_kind, char* FUNC_name){
    enum token_kind buf_token = get_token(read_fp);
    FUNCD* FUNCD_cur = (FUNCD*)malloc(sizeof(FUNCD*));
    strcpy(FUNCD_cur->FUNC_name, FUNC_name);
    FUNCD_cur->FUNC_kind = declare_kind;
    if(buf_token == RP){
        FUNCD_cur->FPL = NULL;
    }
    else if(check_declare(buf_token)){
        FUNCD_cur->FPL = (FPL*)malloc(sizeof(FPL));
        FUNCD_cur->FPL->FP_kind = buf_token;
        buf_token = get_token(read_fp);
        if(buf_token == IDENT){
            strcpy(FUNCD_cur->FPL->FP_name, token_name);
        }
        else{
            errorfound(0);//invalid FPL
        }
        buf_token = get_token(read_fp);
        if(buf_token == COMMA){
            FUNCD_cur->FPL->next = FormParaList(read_fp);
        }
        else if(buf_token == RP){
            FUNCD_cur->FPL->next = NULL;
        }
    }
    return FUNCD_cur;
}

FPL* FormParaList(FILE* read_fp){
    FPL* FPL_cur = (FPL*)malloc(sizeof(FPL));
    enum token_kind buf_token = get_token(read_fp);
    if(check_declare(buf_token)){
        FPL_cur->FP_kind = buf_token;
    }
    else{
        errorfound(0);//invalid FPL
    }
    buf_token = get_token(read_fp);
    if(buf_token == IDENT){
        strcpy(FPL_cur->FP_name, token_name);
    }
    else{
        errorfound(0);//invalid FPL
    }
    buf_token = get_token(read_fp);
    if(buf_token == COMMA){
        FPL_cur->next = FormParaList(read_fp);
    }
    else if(buf_token == RP){
        FPL_cur->next = NULL;
    }
    else{
        errorfound(0);//invalid FPL
    }
    return FPL_cur;
}
