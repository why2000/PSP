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

COMPS* CompStates(RNL* leaveFunRNL, RNL* rootFunRNL){
    COMPS* COMPS_cur = (COMPS*)malloc(sizeof(COMPS));
    COMPS_cur->COMP = (COMP*)malloc(sizeof(COMP));
    enum token_kind buf_token = get_token();
    char name_buf[MAX_TOKEN_SIZE];
    strcpy(name_buf, token_name);
    if(check_declare(buf_token)){
        COMPS_cur->COMP->EVD = (EVD*)malloc(sizeof(EVD));
        enum token_kind buf_kind = buf_token;
        COMPS_cur->COMP->EVD->EVD_kind = buf_kind;
        buf_token = get_token();
        if(buf_token == IDENT){
            COMPS_cur->COMP->EVD->EVNL = (EVNL*)malloc(sizeof(EVNL));
            COMPS_cur->COMP->EVD->EVNL = ExtVarNameList(COMPS_cur->COMP->EVD->EVNL, buf_kind);
        }
        else{
            errorfound(0);// invalid Internal Def
        }
    }
    else if(buf_token == IDENT){
        
    }
    return COMPS_cur;
}
