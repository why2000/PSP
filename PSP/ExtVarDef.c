//
//  ExtVarDef.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/21.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include "ExtVarDef.h"
extern int line_num;//"main.c"
extern char token_name[MAX_TOKEN_SIZE];//"main.c"
extern FILE* read_fp;//"main.c"
extern RNL* rootRNL;//"main.c"
extern RNL* leaveRNL;//"main.c"

EVD* ExtVarDef(enum token_kind declare_kind, char* first_name){
    EVD* EVD_cur = (EVD*)malloc(sizeof(EVD));
    EVD_cur->EVD_kind = declare_kind;
    EVD_cur->EVNL = (EVNL*)malloc(sizeof(EVNL));
    strcpy(EVD_cur->EVNL->var_name, first_name);
    EVD_cur->EVNL->next = (EVNL*)malloc(sizeof(EVNL));
    EVD_cur->EVNL->next = ExtVarNameList(EVD_cur->EVNL->next, declare_kind, &leaveRNL, rootRNL);
    return EVD_cur;
}

EVNL* ExtVarNameList(EVNL* EVNL_cur, enum token_kind declare_kind, RNL** curleaveRNL, RNL* currootRNL){
    enum token_kind token_buf = get_token();
    if(token_buf == IDENT){
        strcpy(EVNL_cur->var_name, token_name);
        if(!search_RNL(currootRNL, token_name)){
            push_RNL(curleaveRNL, token_name, EXTVARDEF, declare_kind);
        }
        else{
            errorfound(1);//has been registered
        }
        token_buf = get_token();
        if(token_buf == COMMA){
            EVNL_cur->next = (EVNL*)malloc(sizeof(EVNL));
            EVNL_cur->next = ExtVarNameList(EVNL_cur->next, declare_kind, curleaveRNL, currootRNL);
            return EVNL_cur;
        }
        else if(token_buf == SEMI){
            EVNL_cur->next = NULL;
            return EVNL_cur;
        }
    }
    else{
        errorfound(0);//invalid EVD
    }
    return NULL;
}

