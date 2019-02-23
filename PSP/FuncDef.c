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
extern FILE* read_fp;//"main.c"
extern RNL* rootRNL;//"main.c"
extern RNL* leaveRNL;//"main.c"

FUNCD* FuncDef(enum token_kind declare_kind, char* FUNC_name){
    RNL* rootFunRNL = leaveRNL;
    RNL* leaveFunRNL = rootFunRNL;
    enum token_kind buf_token = get_token();
    FUNCD* FUNCD_cur = (FUNCD*)malloc(sizeof(FUNCD*));
    strcpy(FUNCD_cur->FUNC_name, FUNC_name);
    FUNCD_cur->FUNC_kind = declare_kind;
    if(buf_token == RP){
        FUNCD_cur->FPL = NULL;
    }
    else if(check_declare(buf_token)){
        FUNCD_cur->FPL = (FPL*)malloc(sizeof(FPL));
        FUNCD_cur->FPL->FP_kind = buf_token;
        buf_token = get_token();
        if(buf_token == IDENT){
            push_RNL(&leaveFunRNL, token_name, EXTVARDEF, FUNCD_cur->FPL->FP_kind);
            strcpy(FUNCD_cur->FPL->FP_name, token_name);
        }
        else{
            errorfound(0);//invalid FPL
        }
        buf_token = get_token();
        if(buf_token == COMMA){
            FUNCD_cur->FPL->next = FormParaList(leaveFunRNL);
        }
        else if(buf_token == RP){
            FUNCD_cur->FPL->next = NULL;
        }
    }
    buf_token = get_token();
    if(buf_token == LB){
        RNL* leaveFunRNL_buf = leaveFunRNL;
        RNL* rootFunRNL_buf = rootFunRNL;
        FUNCD_cur->FUNCB = CompStates(&leaveFunRNL_buf, &rootFunRNL_buf);
    }
    else{
        errorfound(0);//invalid FuncDef
    }
    return FUNCD_cur;
}

FPL* FormParaList(RNL* leaveFunRNL){
    FPL* FPL_cur = (FPL*)malloc(sizeof(FPL));
    enum token_kind buf_token = get_token();
    if(check_declare(buf_token)){
        FPL_cur->FP_kind = buf_token;
    }
    else{
        errorfound(0);//invalid FPL
    }
    buf_token = get_token();
    //形参表不会变量名冲突，一定是可以安全注册的
    if(buf_token == IDENT){
        push_RNL(&leaveFunRNL, token_name, EXTVARDEF, FPL_cur->FP_kind);
        strcpy(FPL_cur->FP_name, token_name);
    }
    else{
        errorfound(0);//invalid FPL
    }
    buf_token = get_token();
    if(buf_token == COMMA){
        FPL_cur->next = FormParaList(leaveFunRNL);
    }
    else if(buf_token == RP){
        FPL_cur->next = NULL;
    }
    else{
        errorfound(0);//invalid FPL
    }
    return FPL_cur;
}
