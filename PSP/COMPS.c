//
//  COMPS.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/22.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include "COMPS.h"
extern int line_num;//"main.c"
extern char token_name[MAX_TOKEN_SIZE];//"main.c"
extern FILE* read_fp;//"main.c"
extern RNL* rootRNL;//"main.c"
extern RNL* leaveRNL;//"main.c"
extern int IF_STATUS;
COMPS* CompStates(RNL** leaveFunRNL, RNL** rootFunRNL){
    COMPS* COMPS_cur = (COMPS*)malloc(sizeof(COMPS));
    COMPS_cur->COMP = (COMP*)malloc(sizeof(COMP));
    COMPS_cur->COMP->STA = NULL;
    COMPS_cur->COMP->EVD = NULL;
    enum token_kind buf_token = get_token();
    char name_buf[MAX_TOKEN_SIZE];
    strcpy(name_buf, token_name);
    if(buf_token == ELSE && IF_STATUS){
        COMPS_cur->COMP_kind = ELSESTA;
        COMPS_cur->COMP->STA = (STA*)malloc(sizeof(STA));
        COMPS_cur->COMP->STA->ELTH = elseState(*leaveFunRNL, *rootFunRNL);
    }
    IF_STATUS = 0;
    if(check_declare(buf_token)){
        COMPS_cur->COMP_kind = EXTVARDEF;
        COMPS_cur->COMP->EVD = (EVD*)malloc(sizeof(EVD));
        enum token_kind buf_kind = buf_token;
        COMPS_cur->COMP->EVD->EVD_kind = buf_kind;
        buf_token = get_token();
        if(buf_token == IDENT){
            COMPS_cur->COMP->EVD->EVNL = (EVNL*)malloc(sizeof(EVNL));
            COMPS_cur->COMP->EVD->EVNL = ExtVarNameList(COMPS_cur->COMP->EVD->EVNL, buf_kind, leaveFunRNL);
        }
        else{
            errorfound(0);// invalid Internal Def
        }
    }
    else if(buf_token == IF){
        COMPS_cur->COMP_kind = IFSTA;
        COMPS_cur->COMP->STA = (STA*)malloc(sizeof(STA));
        COMPS_cur->COMP->STA->IFTH = ifState(*leaveFunRNL, *rootFunRNL);
    }
    else if(buf_token == RET){
        COMPS_cur->COMP_kind = RETURNSTA;
        COMPS_cur->COMP->STA = (STA*)malloc(sizeof(STA));
        COMPS_cur->COMP->STA->RTS = returnState();
    }
    else if(buf_token == RB){
        free(COMPS_cur->COMP);
        free(COMPS_cur);
        COMPS_cur = NULL;
        return COMPS_cur;
    }
    else{
        strcpy(name_buf, token_name);
        COMPS_cur->COMP_kind = NORMSTA;
        COMPS_cur->COMP->STA = (STA*)malloc(sizeof(STA));
        COMPS_cur->COMP->STA->NMS = Statement(buf_token, name_buf);
    }
    COMPS_cur->next = CompStates(leaveFunRNL, rootFunRNL);
    return COMPS_cur;
}
