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
extern long dbugp;
extern int dbugi;

FUNCD* FuncDef(enum token_kind declare_kind, char* FUNC_name){
    RNL* rootFunRNL = leaveRNL;
    RNL* leaveFunRNL = rootFunRNL;
    RNL* bufFunRNL = leaveRNL;
    enum token_kind buf_token = get_token();
    FUNCD* FUNCD_cur = (FUNCD*)malloc(sizeof(FUNCD*));
    FUNCD_cur->FUNCB = NULL;
    FUNCD_cur->FPL = NULL;
    strcpy(FUNCD_cur->FUNC_name, FUNC_name);
    FUNCD_cur->FUNC_kind = declare_kind;
    if(buf_token == RP){
        FUNCD_cur->FPL = (FPL*)malloc(sizeof(FPL));
        FUNCD_cur->FPL->FP_kind = EMPTY_TOKEN;
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
            FUNCD_cur->FPL->next = FormParaList(&leaveFunRNL);
        }
        else if(buf_token == RP){
            FUNCD_cur->FPL->next = (FPL*)malloc(sizeof(FPL));
            FUNCD_cur->FPL->next->FP_kind = EMPTY_TOKEN;
        }
    }
    bufFunRNL->FPL = FUNCD_cur->FPL;
    buf_token = get_token();
    if(buf_token == COMMA){
        FUNCD_cur->FPL->next = FormParaList(&leaveFunRNL);
    }
    if(buf_token == LB){
        RNL* leaveFunRNL_buf = leaveFunRNL;
        RNL* rootFunRNL_buf = rootFunRNL;
        if(buf_token == COMMA){
            FUNCD_cur->FPL->next = FormParaList(&leaveFunRNL);
        }
        COMPS* testdb = CompStates(&leaveFunRNL_buf, &rootFunRNL_buf);
        FUNCD_cur->FUNCB = testdb;
        if(buf_token == COMMA){
            FUNCD_cur->FPL->next = FormParaList(&leaveFunRNL);
        }
        clear_RNL(rootFunRNL_buf);
        if(buf_token == COMMA){
            FUNCD_cur->FPL->next = FormParaList(&leaveFunRNL);
        }
        rootFunRNL_buf->next = NULL;
    }
    else{
        errorfound(0);//invalid FuncDef
    }
    return FUNCD_cur;
}

FPL* FormParaList(RNL** leaveFunRNL){
    FPL* FPL_cur = (FPL*)malloc(sizeof(FPL));
    enum token_kind buf_token = get_token();
    if(check_declare(buf_token)){
        FPL_cur->FP_kind = buf_token;
    }
    else{
        errorfound(0);//invalid FPL
    }
    buf_token = get_token();
    //形参表只会与形参表内部变量名冲突
    if(buf_token == IDENT){
        if(!search_RNL((*leaveFunRNL), token_name)){
            push_RNL(leaveFunRNL, token_name, EXTVARDEF, FPL_cur->FP_kind);
            strcpy(FPL_cur->FP_name, token_name);
        }
        else{
            errorfound(REDUNDANT);
        }
    }
    else{
        errorfound(0);//invalid FPL
    }
    buf_token = get_token();
    if(buf_token == COMMA){
        FPL_cur->next = FormParaList(leaveFunRNL);
    }
    else if(buf_token == RP){
        FPL_cur->next = (FPL*)malloc(sizeof(FPL));
        FPL_cur->next->FP_kind = EMPTY_TOKEN;
    }
    else{
        errorfound(0);//invalid FPL
    }
    return FPL_cur;
}
