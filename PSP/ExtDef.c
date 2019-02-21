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


EDL* ExtDefList(FILE* read_fp){
    enum token_kind tk_cur;
    tk_cur = get_token(read_fp);
    EDL* EDL_cur = (EDL*)malloc(sizeof(EDL));
    if(tk_cur == EMPTY_TOKEN){
        return NULL;
    }
    else if(check_declare(tk_cur)){
        ED* buf_ED = ExtDef(read_fp);
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
    else{
        errorfound(0);//invalid ExtDef
    }
    EDL_cur->EDL = ExtDefList(read_fp);
    
    //    cur_tk = get_token(read_fp);
    
    return EDL_cur;
}


ED* ExtDef(FILE* read_fp){
    enum token_kind token_buf = get_token(read_fp);
    enum token_kind kind_buf;
    char ED_name[MAX_TOKEN_SIZE];
    ED* ED_cur = (ED*)malloc(sizeof(ED));
    if(check_declare(token_buf)){
        kind_buf = token_buf;
        token_buf = get_token(read_fp);
        if(token_buf == IDENT){
            strcpy(ED_name, token_name);
        }
        else{
            errorfound(0);//invalid ED
        }
        token_buf = get_token(read_fp);
        if(token_buf == LP){
            ED_cur->FUNCD = NULL;
        }
        else if(token_buf == COMMA){
            ED_cur->EVD = ExtVarDef(read_fp, kind_buf, ED_name);
        }
          
    }
    else if(token_buf == SEMI){
        ED_cur = ExtDef(read_fp);
    }
    else if(token_buf == EMPTY_TOKEN){
        ED_cur->EVD = NULL;
        ED_cur->FUNCD = NULL;
    }
    else{
        errorfound(0);//invalid ED
    }
    return ED_cur;
}













