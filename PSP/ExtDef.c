//
//  ExtDef.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include "ExtDef.h"
extern int line_num;//"main.c"
extern char token_name[MAX_TOKEN_SIZE];//"main.c"
extern FILE* read_fp;//"main.c"
extern RNL* rootRNL;//"main.c"
extern RNL* leaveRNL;//"main.c"

EDL* ExtDefList(void){
    enum token_kind tk_cur;
    tk_cur = get_token();
    EDL* EDL_cur = (EDL*)malloc(sizeof(EDL));
    if(tk_cur == EMPTY_TOKEN){
        return NULL;
    }
    else if(check_declare(tk_cur)){
        ED* buf_ED = ExtDef(tk_cur);
        if(buf_ED->EVD != NULL){
            EDL_cur->ED_kind = EXTVARDEF;
            EDL_cur->ED = buf_ED;
        }
        else if(buf_ED->FUNCD != NULL){
            EDL_cur->ED_kind = FUNCDEF;
            EDL_cur->ED = buf_ED;
        }
        else{
            return NULL;
        }
    }
    else if(tk_cur == SEMI){
        EDL_cur = ExtDefList();
    }
    else{
        errorfound(0);//invalid ExtDef
    }
    EDL_cur->EDL = ExtDefList();
    
    //    cur_tk = get_token(read_fp);
    
    return EDL_cur;
}


ED* ExtDef(enum token_kind kind_buf){
    enum token_kind token_buf;
    char ED_name[MAX_TOKEN_SIZE];
    ED* ED_cur = (ED*)malloc(sizeof(ED));
    token_buf = get_token();
    if(token_buf == IDENT){
        strcpy(ED_name, token_name);
    }
    else{
        errorfound(0);//invalid ED
    }
    token_buf = get_token();
    if(token_buf == LP){
        push_RNL(&leaveRNL, ED_name, FUNCDEF, kind_buf);
        ED_cur->FUNCD = FuncDef(kind_buf, ED_name);
    }
    else if(token_buf == COMMA){
        push_RNL(&leaveRNL, ED_name, EXTVARDEF, kind_buf);
        ED_cur->EVD = ExtVarDef(kind_buf, ED_name);
    }
    else if(token_buf == SEMI){
        push_RNL(&leaveRNL, ED_name, EXTVARDEF, kind_buf);
        ED_cur->EVD = (EVD*)malloc(sizeof(EVD));
        ED_cur->EVD->EVNL = (EVNL*)malloc(sizeof(EVNL));
        ED_cur->EVD->EVD_kind = kind_buf;
        strcpy(ED_cur->EVD->EVNL->var_name, ED_name);
        ED_cur->EVD->EVNL->next = NULL;
    }
    else{
        errorfound(0);//invalid ED
    }
    return ED_cur;
}













