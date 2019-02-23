//
//  StateList.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/22.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include "Statement.h"
extern int line_num;//"main.c"
extern char token_name[MAX_TOKEN_SIZE];//"main.c"
extern FILE* read_fp;//"main.c"
extern RNL* rootRNL;//"main.c"
extern RNL* leaveRNL;//"main.c"
extern int IF_STATUS;

// 处理常规表达式及函数调用
// 注意if status = 1时允许多一个小括号，并且此时结束表达式
NMS* Statement(enum token_kind first_token, char* first_name, RNL* leaveCurRNL, RNL* rootCurRNL){
    enum token_kind token_buf = first_token;
    REPOL* repol = (REPOL*)malloc(sizeof(REPOL));
    REPOL* repol_root = repol;
    repol->APL = NULL;
    repol->next = NULL;
    repol->token_kind = EMPTY_TOKEN;
    SOP* sop = (SOP*)malloc(sizeof(SOP));
    sop->last = NULL;
    sop->next = NULL;
    sop->op_kind = EMPTY_TOKEN;
    NMS* sout = (NMS*)malloc(sizeof(NMS));
    sout->left = NULL;
    sout->right = NULL;
    sout->NM_kind = EMPTY_TOKEN;
    sout->APL = NULL;
    while (token_buf != SEMI) {
        //1.括号处理
        if (token_buf == COMMA && (get_stack(&sop) == EMPTY_TOKEN))break;
        if(token_buf == RP && (get_stack(&sop) == EMPTY_TOKEN)){
            ungetc(')', read_fp);
            break;
        }
        if((token_buf == RP) && IF_STATUS){
            break;
        }
        if (token_buf == LP) {//左括号无视优先级直接入栈
            push_stack(&sop, token_buf);
            token_buf = get_token();
        }
        else if (token_buf == RP) {//注意右括号本身永远不会存在于sop栈中，后续不用判断其存在
            //左括号上方符号出栈
            while ((get_stack(&sop) != LP) && (sop->last != NULL)) {
                push_list(&repol, pop_stack(&sop), "\0");
            }
            //*************************错误1：右括号多于左括号********************************
            if (sop->last == NULL) {
                    errorfound(1);
            }
            //********************************ERROR1**************************************
            sop = sop->last;//舍弃左括号
            //舍弃右括号
            token_buf = get_token();
        }
        //2.读取运算符
        else if (check_operator(token_buf)) {//同级或高级运算符及其上方的全部运算符出栈压入repol，再将本身压入sop栈
            while (sop->last != NULL && compare_operator(get_stack(&sop), token_buf)) {
                push_list(&repol, pop_stack(&sop), "\0");
            }
            push_stack(&sop, token_buf);
            token_buf = get_token();
        }
        else if(token_buf == IDENT) {
            enum token_kind fun_buf = token_buf;
            char fun_name[MAX_TOKEN_SIZE];
            strcpy(fun_name, token_name);
            token_buf = get_token();
            if(token_buf == LP){
                token_buf = get_token();
                char first_name[MAX_TOKEN_SIZE];
                strcpy(first_name, token_name);
                APL* APL_cur = funCall(token_buf, first_name, leaveCurRNL, rootCurRNL);
                push_list(&repol, FUNC, fun_name);
                repol->APL = APL_cur;
                if((repol->res_kind = check_registed(rootRNL, fun_name, FUNCDEF, APL_cur)) == EMPTY_TOKEN){
                    errorfound(1);//unregisted
                }
                token_buf = get_token();
            }
            else{
                push_list(&repol, fun_buf, fun_name);
                if((repol->res_kind = check_registed(rootRNL, fun_name, EXTVARDEF, NULL)) == EMPTY_TOKEN){
                    errorfound(1);//unregisted
                }
            }
        }
        else if(check_const(token_buf)){//直接写入逆波兰栈
            push_list(&repol, token_buf, token_name);
            repol->res_kind = check_const(token_buf);
            token_buf = get_token();
        }
        //**************************错误6：无法识别的输入**************************************
        else {
            errorfound(6);
        }
        //**********************************ERROR6*****************************************
    }
    while (sop->last != NULL) {//符号压入repol
        //***********************错误2：左括号多于右括号************************************
        if (get_stack(&sop) == LP) {
            errorfound(0);
        }
        //******************************ERROR2******************************************
        push_list(&repol, pop_stack(&sop), "\0");
    }
    push_list(&repol, EMPTY_TOKEN, "\0");//逆波兰栈写入完成
    repol = repol_root->next;
    free(repol_root);
    repol_root = NULL;
    return geneNMS(&repol, &sout);
}

NMS* geneNMS(REPOL** repol, NMS** sout){
    while((*repol)->token_kind != EMPTY_TOKEN){
        if(((*repol)->token_kind == IDENT) || ((*repol)->token_kind == FUNC) || check_const((*repol)->token_kind)){
            push_out(sout, (*repol));
        }
        else{
            //链表转向
            NMS* NMS_cur = (NMS*)malloc(sizeof(NMS));
            NMS_cur->NM_kind = (*repol)->token_kind;
            //此处不会泄露，next与last均已被横向链表引用
            NMS_cur->right = pop_out(sout);
            NMS_cur->right->next = NULL;
            NMS_cur->right->last = NULL;
            //注意赋值系列的左操作符
            NMS_cur->left = pop_out(sout);
            NMS_cur->left->next = NULL;
            NMS_cur->left->last = NULL;
            if(NMS_cur->left->res_kind != NMS_cur->right->res_kind){
                errorfound(1);//左右操作数类型不符
            }
            if(order_operator(NMS_cur->NM_kind) == 14){
                if(NMS_cur->left->NM_kind != IDENT){
                    errorfound(1);//assign is only for ident
                }
            }
            NMS* sout_buf = (*sout);
            (*sout)->next = (NMS*)malloc(sizeof(NMS));
            (*sout) = (*sout)->next;
            (**sout) = *NMS_cur;
            (*sout)->last = sout_buf;
            (*sout)->next = NULL;
            sout_buf = NULL;
        }
        REPOL* repol_buf = (*repol)->next;
        free(*repol);
        (*repol) = repol_buf;
    }
    free((*sout)->last);
    (*sout)->last = NULL;
    return (*sout);
}

void push_stack(SOP** sop, enum token_kind op_kind){
    SOP* buf_sop = (*sop);
    (*sop)->next = (SOP*)malloc(sizeof(SOP));
    (*sop) = (*sop)->next;
    (*sop)->last = buf_sop;
    (*sop)->op_kind = op_kind;
    (*sop)->next = NULL;
}

enum token_kind get_stack(SOP** sop){
    return (*sop)->op_kind;
}

enum token_kind pop_stack(SOP** sop){
    SOP* buf_sop = (*sop);
    enum token_kind token_buf = (*sop)->op_kind;
    (*sop) = (*sop)->last;
    free(buf_sop);
    (*sop)->next = NULL;
    return token_buf;
}

void push_list(REPOL** repol, enum token_kind token_kind, char* buf_name){
    (*repol)->next = (REPOL*)malloc(sizeof(REPOL));
    (*repol) = (*repol)->next;
    (*repol)->token_kind = token_kind;
    (*repol)->APL = NULL;
    strcpy((*repol)->token_name, buf_name);
    (*repol)->next = NULL;
}

NMS* pop_out(NMS** sout){
    NMS* buf_sout = (*sout); 
    (*sout) = (*sout)->last;
    (*sout)->next = NULL;
    return buf_sout;
}

void push_out(NMS** sout, REPOL* in_repol){
    NMS* buf_sout = (*sout);
    (*sout)->next = (NMS*)malloc(sizeof(NMS));
    (*sout) = (*sout)->next;
    (*sout)->NM_kind = in_repol->token_kind;
    (*sout)->APL = in_repol->APL;
    (*sout)->res_kind = in_repol->res_kind;
    strcpy((*sout)->NMS_name, in_repol->token_name);
    (*sout)->next = NULL;
    (*sout)->last = buf_sout;
    (*sout)->left = NULL;
    (*sout)->right = NULL;
}


APL* funCall(enum token_kind first_token, char* first_name, RNL* leaveCurRNL, RNL* rootCurRNL){
    if(first_token == RP){
        return NULL;
    }
    APL* APL_cur = (APL*)malloc(sizeof(APL));
    APL_cur->NMS = Statement(first_token, first_name, rootCurRNL, leaveCurRNL);
    enum token_kind token_buf;
    char name_buf[MAX_TOKEN_SIZE];
    token_buf = get_token();
    strcpy(name_buf, token_name);
    APL_cur->next = funCall(token_buf, name_buf, rootCurRNL, leaveCurRNL);
    return APL_cur;
}

IFTH* ifState(RNL* leaveFunRNL, RNL* rootFunRNL){
    IFTH* IFTH_cur = (IFTH*)malloc(sizeof(IFTH));
    enum token_kind token_buf = get_token();
    char name_buf[MAX_TOKEN_SIZE];
    if(token_buf == LP){
        token_buf = get_token();
        strcpy(name_buf, token_name);
        IFTH_cur->NMS = Statement(token_buf, name_buf, leaveFunRNL, rootFunRNL);
        token_buf = get_token();
        if(token_buf == LB){
            RNL* leaveFunRNL_buf = leaveFunRNL;
            RNL* rootFunRNL_buf = rootFunRNL;
            IF_STATUS = 0;
            IFTH_cur->IFCOMPS = CompStates(&leaveFunRNL_buf, &rootFunRNL_buf);
            IF_STATUS = 1;
        }
        else{
            strcpy(name_buf, token_name);
            IFTH_cur->IFCOMPS = (COMPS*)malloc(sizeof(COMPS));
            IFTH_cur->IFCOMPS->COMP_kind = NORMSTA;
            IFTH_cur->IFCOMPS->COMP = (COMP*)malloc(sizeof(COMP));
            IF_STATUS = 0;
            IFTH_cur->IFCOMPS->COMP->STA = (STA*)malloc(sizeof(STA));
            IFTH_cur->IFCOMPS->COMP->STA->NMS = Statement(token_buf, name_buf, leaveFunRNL, rootFunRNL);
            IF_STATUS = 1;
            IFTH_cur->IFCOMPS->next = NULL;
        }
    }
    else{
        errorfound(0);// invalid if statement
    }
    return IFTH_cur;
}

ELTH* elseState(RNL* leaveFunRNL, RNL* rootFunRNL){
    ELTH* ELTH_cur = (ELTH*)malloc(sizeof(ELTH));
    enum token_kind token_buf = get_token();
    char name_buf[MAX_TOKEN_SIZE];
    if(token_buf == LB){
        RNL* leaveFunRNL_buf = leaveFunRNL;
        RNL* rootFunRNL_buf = rootFunRNL;
        ELTH_cur->ELSECOMPS = CompStates(&leaveFunRNL_buf, &rootFunRNL_buf);
    }
    else{
        strcpy(name_buf, token_name);
        ELTH_cur->ELSECOMPS = (COMPS*)malloc(sizeof(COMPS));
        ELTH_cur->ELSECOMPS->COMP_kind = NORMSTA;
        ELTH_cur->ELSECOMPS->COMP = (COMP*)malloc(sizeof(COMP));
        ELTH_cur->ELSECOMPS->COMP->STA = (STA*)malloc(sizeof(STA));
        ELTH_cur->ELSECOMPS->COMP->STA->NMS = Statement(token_buf, name_buf, leaveFunRNL, rootFunRNL);
        ELTH_cur->ELSECOMPS->next = NULL;
    }
    return ELTH_cur;
}


RTS* returnState(RNL* leaveFunRNL, RNL* rootFunRNL){
    RTS* RTS_cur = (RTS*)malloc(sizeof(RTS));
    char name_buf[MAX_TOKEN_SIZE];
    enum token_kind token_buf = get_token();
    strcpy(name_buf, token_name);
    RTS_cur->NMS = Statement(token_buf, name_buf, leaveFunRNL, rootFunRNL);
    return RTS_cur;
}










