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
extern FILE* write_fp;
extern long dbugp;
extern int dbugi;
COMPS* CompStates(RNL** leaveFunRNL, RNL** rootFunRNL){
    COMPS* COMPS_cur = (COMPS*)malloc(sizeof(COMPS));
    COMPS_cur->COMP = (COMP*)malloc(sizeof(COMP));
    COMPS_cur->COMP_kind = ENDOFSTA;
    COMPS_cur->COMP->STA = NULL;
    COMPS_cur->COMP->EVD = NULL;
    enum token_kind buf_token = get_token();
    char name_buf[MAX_TOKEN_SIZE];
    strcpy(name_buf, token_name);
    if(check_declare(buf_token)){
        IF_STATUS = 0;
        COMPS_cur->COMP_kind = EXTVARDEF;
        COMPS_cur->COMP->EVD = (EVD*)malloc(sizeof(EVD));
        enum token_kind buf_kind = buf_token;
        COMPS_cur->COMP->EVD->EVD_kind = buf_kind;
        COMPS_cur->COMP->EVD->EVNL = (EVNL*)malloc(sizeof(EVNL));
        COMPS_cur->COMP->EVD->EVNL = ExtVarNameList(buf_kind, leaveFunRNL, (*rootFunRNL));
//        EVNLoutput(COMPS_cur->COMP->EVD->EVNL);
        COMPS_cur->COMP->STA = NULL;
    }
    else if(buf_token == IF){
        COMPS_cur->COMP_kind = IFSTA;
        COMPS_cur->COMP->STA = (STA*)malloc(sizeof(STA));
        COMPS_cur->COMP->STA->IFTH = ifState(*leaveFunRNL, *rootFunRNL);
        COMPS_cur->COMP->STA->WHILETH = NULL;
        COMPS_cur->COMP->STA->FORTH = NULL;
        COMPS_cur->COMP->STA->RTS = NULL;
        COMPS_cur->COMP->STA->ELTH = NULL;
        COMPS_cur->COMP->STA->NMS = NULL;
        COMPS_cur->COMP->EVD = NULL;
        IF_STATUS = 1;
    }
    else if(buf_token == WHILE){
        IF_STATUS = 0;
        COMPS_cur->COMP_kind = WHILESTA;
        COMPS_cur->COMP->STA = (STA*)malloc(sizeof(STA));
        COMPS_cur->COMP->STA->WHILETH = whileState(*leaveFunRNL, *rootFunRNL);
        COMPS_cur->COMP->STA->FORTH = NULL;
        COMPS_cur->COMP->STA->RTS = NULL;
        COMPS_cur->COMP->STA->ELTH = NULL;
        COMPS_cur->COMP->STA->NMS = NULL;
        COMPS_cur->COMP->EVD = NULL;
    }
    else if(buf_token == RET){
        IF_STATUS = 0;
        COMPS_cur->COMP_kind = RETURNSTA;
        COMPS_cur->COMP->STA = (STA*)malloc(sizeof(STA));
        COMPS_cur->COMP->STA->RTS = returnState(*leaveFunRNL, *rootFunRNL);
        COMPS_cur->COMP->STA->IFTH = NULL;
        COMPS_cur->COMP->STA->ELTH = NULL;
        COMPS_cur->COMP->STA->NMS = NULL;
        COMPS_cur->COMP->STA->WHILETH = NULL;
        COMPS_cur->COMP->STA->FORTH = NULL;
        COMPS_cur->COMP->EVD = NULL;
    }
    else if(buf_token == RB){
        return COMPS_cur;
    }
    else if(buf_token == FOR){
        IF_STATUS = 0;
        COMPS_cur->COMP_kind = FORSTA;
        COMPS_cur->COMP->STA = (STA*)malloc(sizeof(STA));
        COMPS_cur->COMP->STA->FORTH = forState(*leaveFunRNL, *rootFunRNL);
        COMPS_cur->COMP->STA->IFTH = NULL;
        COMPS_cur->COMP->STA->ELTH = NULL;
        COMPS_cur->COMP->STA->NMS = NULL;
        COMPS_cur->COMP->STA->WHILETH = NULL;
        COMPS_cur->COMP->STA->RTS = NULL;
        COMPS_cur->COMP->EVD = NULL;
    }
    else if(buf_token == ELSE){
        if(IF_STATUS){
            IF_STATUS = 0;
            COMPS_cur->COMP_kind = ELSESTA;
            COMPS_cur->COMP->STA = (STA*)malloc(sizeof(STA));
            COMPS_cur->COMP->STA->ELTH = elseState(*leaveFunRNL, *rootFunRNL);
            COMPS_cur->COMP->STA->RTS = NULL;
            COMPS_cur->COMP->STA->IFTH = NULL;
            COMPS_cur->COMP->STA->NMS = NULL;
            COMPS_cur->COMP->STA->WHILETH = NULL;
            COMPS_cur->COMP->STA->FORTH = NULL;
            COMPS_cur->COMP->EVD = NULL;
        }
        else{
            errorfound(ELSEONLY);//else without if
        }
    }
    else{
        IF_STATUS = 0;
        strcpy(name_buf, token_name);
        COMPS_cur->COMP_kind = NORMSTA;
        COMPS_cur->COMP->STA = (STA*)malloc(sizeof(STA));
        COMPS_cur->COMP->STA->NMS = Statement(buf_token, name_buf, (*leaveFunRNL), (*rootFunRNL));
        COMPS_cur->COMP->STA->RTS = NULL;
        COMPS_cur->COMP->STA->ELTH = NULL;
        COMPS_cur->COMP->STA->IFTH = NULL;
        COMPS_cur->COMP->STA->WHILETH = NULL;
        COMPS_cur->COMP->STA->FORTH = NULL;
        COMPS_cur->COMP->EVD = NULL;
    }
    COMPS_cur->next = CompStates(leaveFunRNL, rootFunRNL);
    return COMPS_cur;
}
