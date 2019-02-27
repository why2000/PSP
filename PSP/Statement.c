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
extern int JUDGE_STATUS;

// 处理常规表达式及函数调用
// 注意if status = 1时允许多一个小括号，并且此时结束表达式
// sop中全部为本地变量，可以使用栈指针
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
        if((token_buf == RP) && JUDGE_STATUS){
            break;
        }
        if(token_buf == RP && (get_stack(&sop) == EMPTY_TOKEN)){
            ungetc(')', read_fp);
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
            if (sop->last == NULL) {
                    errorfound(TMRIGHT);//右括号多于左括号
            }
            sop = sop->last;//舍弃左括号
            //舍弃右括号
            token_buf = get_token();
        }
        //2.读取运算符
        else if (check_operator(token_buf) && !check_oneside(token_buf)) {//同级或高级运算符及其上方的全部运算符出栈压入repol，再将本身压入sop栈
            while (sop->last != NULL && compare_operator(get_stack(&sop), token_buf)) {
                push_list(&repol, pop_stack(&sop), "\0");
            }
            if (token_buf == COMMA && (get_stack(&sop) == EMPTY_TOKEN))break;
            push_stack(&sop, token_buf);
            token_buf = get_token();
        }
        else if(check_oneside(token_buf)){//单目运算符先检测左右
            if(token_buf == DBPLUS){
                token_buf = get_token();
                if(!check_bothside(token_buf)){
                    while (sop->last != NULL && compare_operator(get_stack(&sop), LDBPLUS)) {
                        push_list(&repol, pop_stack(&sop), "\0");
                    }
                    if(get_stack(&sop) == EMPTY_TOKEN)break;
                    push_stack(&sop, LDBPLUS);
                }
                else{
                    while (sop->last != NULL && compare_operator(get_stack(&sop), RDBPLUS)) {
                        push_list(&repol, pop_stack(&sop), "\0");
                    }
                    if(get_stack(&sop) == EMPTY_TOKEN)break;
                    push_stack(&sop, RDBPLUS);
                }
            }
            else if(token_buf == DBMINUS){
                token_buf = get_token();
                if(!check_bothside(token_buf)){
                    while (sop->last != NULL && compare_operator(get_stack(&sop), LDBMINUS)) {
                        push_list(&repol, pop_stack(&sop), "\0");
                    }
                    if(get_stack(&sop) == EMPTY_TOKEN)break;
                    push_stack(&sop, LDBMINUS);
                }
                else{
                    while (sop->last != NULL && compare_operator(get_stack(&sop), RDBMINUS)) {
                        push_list(&repol, pop_stack(&sop), "\0");
                    }
                    if(get_stack(&sop) == EMPTY_TOKEN)break;
                    push_stack(&sop, RDBMINUS);
                }
            }
            else{
                while (sop->last != NULL && compare_operator(get_stack(&sop), token_buf)) {
                    push_list(&repol, pop_stack(&sop), "\0");
                }
                if(get_stack(&sop) == EMPTY_TOKEN)break;
                push_stack(&sop, token_buf);
                token_buf = get_token();
            }
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
                if((repol->res_kind = check_registed(leaveCurRNL, fun_name, FUNCDEF, APL_cur)) == EMPTY_TOKEN){
                    errorfound(UNREGISTED);//unregisted
                }
                token_buf = get_token();
            }
            else{
                push_list(&repol, fun_buf, fun_name);
                if((repol->res_kind = check_registed(leaveCurRNL, fun_name, EXTVARDEF, NULL)) == EMPTY_TOKEN){
                    errorfound(UNREGISTED);//unregisted
                }
            }
        }
        else if(check_const(token_buf)){//直接写入逆波兰栈
            push_list(&repol, token_buf, token_name);
            repol->res_kind = check_const(token_buf);
            token_buf = get_token();
        }
        else {
            errorfound(SYNERR);//未知符号
        }
    }
    while (sop->last != NULL) {//符号压入repol
        if (get_stack(&sop) == LP) {
            errorfound(TMLEFT);//左括号多于右括号
        }
        push_list(&repol, pop_stack(&sop), "\0");
    }
    push_list(&repol, EMPTY_TOKEN, "\0");//逆波兰栈写入完成
    repol = repol_root->next;
    free(repol_root);
    repol_root = NULL;
    return geneNMS(&repol, &sout);
}

NMS* geneNMS(REPOL** repol, NMS** sout){
    NMS* empty_NMS = (NMS*)malloc(sizeof(NMS));
    empty_NMS->NM_kind = EMPTY_TOKEN;
    empty_NMS->res_kind = EMPTY_TOKEN;
    while((*repol)->token_kind != EMPTY_TOKEN){
        if(((*repol)->token_kind == IDENT) || ((*repol)->token_kind == FUNC) || check_const((*repol)->token_kind)){
            push_out(sout, (*repol));
        }
        else{
            //链表转向
            NMS* NMS_cur = (NMS*)malloc(sizeof(NMS));
            NMS_cur->NM_kind = (*repol)->token_kind;
            if(check_oneside(NMS_cur->NM_kind)){
                if(NMS_cur->NM_kind == RDBPLUS || NMS_cur->NM_kind == RDBMINUS){
                    NMS_cur->left = pop_out(sout);
                    NMS_cur->left->next = (NMS*)malloc(sizeof(NMS));
                    NMS_cur->left->next->NM_kind = EMPTY_TOKEN;
                    NMS_cur->left->next->res_kind = EMPTY_TOKEN;
                    NMS_cur->left->last = (NMS*)malloc(sizeof(NMS));
                    NMS_cur->left->last->NM_kind = EMPTY_TOKEN;
                    NMS_cur->left->last->res_kind = EMPTY_TOKEN;
                    NMS_cur->right = (NMS*)malloc(sizeof(NMS));
                    NMS_cur->right->NM_kind = EMPTY_TOKEN;
                    NMS_cur->right->res_kind = EMPTY_TOKEN;
                    NMS_cur->right = (NMS*)malloc(sizeof(NMS));
                    NMS_cur->right->NM_kind = EMPTY_TOKEN;
                    NMS_cur->right->res_kind = EMPTY_TOKEN;
                    NMS_cur->res_kind = NMS_cur->left->res_kind;
                    if(NMS_cur->left->NM_kind != IDENT){
                        errorfound(ASSIGNERR);//特殊操作符只能作用于左值表达式
                    }
                }
                else if(NMS_cur->NM_kind == LDBPLUS || NMS_cur->NM_kind == LDBMINUS){
                    NMS_cur->right = pop_out(sout);
                    NMS_cur->right->next = (NMS*)malloc(sizeof(NMS));
                    NMS_cur->right->next->NM_kind = EMPTY_TOKEN;
                    NMS_cur->right->next->res_kind = EMPTY_TOKEN;
                    NMS_cur->right->last = (NMS*)malloc(sizeof(NMS));
                    NMS_cur->right->last->NM_kind = EMPTY_TOKEN;
                    NMS_cur->right->last->res_kind = EMPTY_TOKEN;
                    NMS_cur->left = (NMS*)malloc(sizeof(NMS));
                    NMS_cur->left->NM_kind = EMPTY_TOKEN;
                    NMS_cur->left->res_kind = EMPTY_TOKEN;
                    NMS_cur->left = (NMS*)malloc(sizeof(NMS));
                    NMS_cur->left->NM_kind = EMPTY_TOKEN;
                    NMS_cur->left->res_kind = EMPTY_TOKEN;
                    NMS_cur->res_kind = NMS_cur->right->res_kind;
                    if(NMS_cur->right->NM_kind != IDENT){
                        errorfound(ASSIGNERR);//特殊操作符只能作用于左值表达式
                    }
                }
                else if(NMS_cur->NM_kind == NOT || NMS_cur->NM_kind == REVERSE){
                    NMS_cur->right = pop_out(sout);
                    NMS_cur->right->next = (NMS*)malloc(sizeof(NMS));
                    NMS_cur->right->next->NM_kind = EMPTY_TOKEN;
                    NMS_cur->right->next->res_kind = EMPTY_TOKEN;
                    NMS_cur->right->last = (NMS*)malloc(sizeof(NMS));
                    NMS_cur->right->last->NM_kind = EMPTY_TOKEN;
                    NMS_cur->right->last->res_kind = EMPTY_TOKEN;
                    NMS_cur->left = (NMS*)malloc(sizeof(NMS));
                    NMS_cur->left->NM_kind = EMPTY_TOKEN;
                    NMS_cur->left->res_kind = EMPTY_TOKEN;
                    NMS_cur->left = (NMS*)malloc(sizeof(NMS));
                    NMS_cur->left->NM_kind = EMPTY_TOKEN;
                    NMS_cur->left->res_kind = EMPTY_TOKEN;
                    NMS_cur->res_kind = NMS_cur->right->res_kind;
                }
                else{
                    errorfound(ERRONESIDE);//单目运算符只有一侧能取值
                }
                
            }else{
                //此处不会泄露，next与last均已被横向链表引用
                NMS_cur->right = pop_out(sout);
                NMS_cur->right->next = (NMS*)malloc(sizeof(NMS));
                NMS_cur->right->next->NM_kind = EMPTY_TOKEN;
                NMS_cur->right->next->res_kind = EMPTY_TOKEN;
                NMS_cur->right->last = (NMS*)malloc(sizeof(NMS));
                NMS_cur->right->last->NM_kind = EMPTY_TOKEN;
                NMS_cur->right->last->res_kind = EMPTY_TOKEN;
                //注意赋值系列的左操作符
                NMS_cur->left = pop_out(sout);
                NMS_cur->left->next = (NMS*)malloc(sizeof(NMS));
                NMS_cur->left->next->NM_kind = EMPTY_TOKEN;
                NMS_cur->left->next->res_kind = EMPTY_TOKEN;
                NMS_cur->left->last = (NMS*)malloc(sizeof(NMS));
                NMS_cur->left->last->NM_kind = EMPTY_TOKEN;
                NMS_cur->left->last->res_kind = EMPTY_TOKEN;
                if((NMS_cur->left->res_kind == NMS_cur->right->res_kind) && (NMS_cur->left->res_kind != EMPTY_TOKEN)){
                    NMS_cur->res_kind = NMS_cur->left->res_kind;
                }
                else{
                    errorfound(DIFFOP);//左右操作数类型不符
                }
            }
            if(NMS_cur->NM_kind == DOT || NMS_cur->NM_kind == CHOOSE){
                errorfound(UNSUPPORTEDYET);
            }
            if(check_bool(NMS_cur->NM_kind)){
                NMS_cur->res_kind = INT;
            }
            if(order_operator(NMS_cur->NM_kind) == 14){
                if(NMS_cur->left->NM_kind != IDENT){
                    errorfound(ASSIGNERR);//只能对左值表达式赋值
                }
            }
            NMS* sout_buf = (*sout);
            (*sout)->next = (NMS*)malloc(sizeof(NMS));
            (*sout) = (*sout)->next;
            (**sout) = *NMS_cur;
            (*sout)->last = sout_buf;
            (*sout)->next = (NMS*)malloc(sizeof(NMS));
            (*sout)->next->NM_kind = EMPTY_TOKEN;
            (*sout)->next->res_kind = EMPTY_TOKEN;
            sout_buf = NULL;
        }
        REPOL* repol_buf = (*repol)->next;
        free(*repol);
        (*repol) = repol_buf;
    }
    (*sout)->last = (NMS*)malloc(sizeof(NMS));
    (*sout)->last->NM_kind = EMPTY_TOKEN;
    (*sout)->last->res_kind = EMPTY_TOKEN;
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
    buf_sop = NULL;
    (*sop)->next = NULL;
    return token_buf;
}

void push_list(REPOL** repol, enum token_kind token_kind, char* buf_name){
    (*repol)->next = (REPOL*)malloc(sizeof(REPOL));
    (*repol) = (*repol)->next;
    (*repol)->token_kind = token_kind;
    (*repol)->APL = NULL;
    strcpy((*repol)->token_name, buf_name);
    (*repol)->next = (REPOL*)malloc(sizeof(REPOL));
    (*repol)->next->res_kind = EMPTY_TOKEN;
    (*repol)->next->token_kind = EMPTY_TOKEN;
}

NMS* pop_out(NMS** sout){
    NMS* buf_sout = (*sout); 
    (*sout) = (*sout)->last;
    (*sout)->next = (NMS*)malloc(sizeof(NMS));
    (*sout)->next->NM_kind = EMPTY_TOKEN;
    (*sout)->next->res_kind = EMPTY_TOKEN;
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
    (*sout)->next = (NMS*)malloc(sizeof(NMS));
    (*sout)->next->NM_kind = EMPTY_TOKEN;
    (*sout)->next->res_kind = EMPTY_TOKEN;
    (*sout)->last = buf_sout;
    (*sout)->left = (NMS*)malloc(sizeof(NMS));
    (*sout)->left->NM_kind = EMPTY_TOKEN;
    (*sout)->left->res_kind = EMPTY_TOKEN;
    (*sout)->next = (NMS*)malloc(sizeof(NMS));
    (*sout)->next->NM_kind = EMPTY_TOKEN;
    (*sout)->next->res_kind = EMPTY_TOKEN;
}


APL* funCall(enum token_kind first_token, char* first_name, RNL* leaveCurRNL, RNL* rootCurRNL){
    if(first_token == RP){
        APL* empty_APL = (APL*)malloc(sizeof(APL));
        empty_APL->NMS = (NMS*)malloc(sizeof(NMS));
        empty_APL->NMS->NM_kind = EMPTY_TOKEN;
        empty_APL->NMS->res_kind = EMPTY_TOKEN;
        return empty_APL;
    }
    APL* APL_cur = (APL*)malloc(sizeof(APL));
    APL_cur->NMS = Statement(first_token, first_name, leaveCurRNL, rootCurRNL);
    enum token_kind token_buf;
    char name_buf[MAX_TOKEN_SIZE];
    token_buf = get_token();
    strcpy(name_buf, token_name);
    APL_cur->next = funCall(token_buf, name_buf, leaveCurRNL, rootCurRNL);
    return APL_cur;
}

IFTH* ifState(RNL* leaveFunRNL, RNL* rootFunRNL){
    IFTH* IFTH_cur = (IFTH*)malloc(sizeof(IFTH));
    enum token_kind token_buf = get_token();
    char name_buf[MAX_TOKEN_SIZE];
    if(token_buf == LP){
        token_buf = get_token();
        strcpy(name_buf, token_name);
        JUDGE_STATUS = 1;
        IFTH_cur->NMS = Statement(token_buf, name_buf, leaveFunRNL, rootFunRNL);
        JUDGE_STATUS = 0;
        token_buf = get_token();
        if(token_buf == LB){
            RNL* leaveFunRNL_buf = leaveFunRNL;
            RNL* rootFunRNL_buf = leaveFunRNL;
            IFTH_cur->IFCOMPS = CompStates(&leaveFunRNL_buf, &rootFunRNL_buf);
            clear_RNL(rootFunRNL_buf);
        }
        else{
            strcpy(name_buf, token_name);
            IFTH_cur->IFCOMPS = (COMPS*)malloc(sizeof(COMPS));
            IFTH_cur->IFCOMPS = instantState(token_buf, name_buf, leaveFunRNL, rootFunRNL);
        }
    }
    else{
        errorfound(SYNERR);// invalid if statement
    }
    return IFTH_cur;
}


ELTH* elseState(RNL* leaveFunRNL, RNL* rootFunRNL){
    ELTH* ELTH_cur = (ELTH*)malloc(sizeof(ELTH));
    enum token_kind token_buf = get_token();
    char name_buf[MAX_TOKEN_SIZE];
    if(token_buf == LB){
        RNL* leaveFunRNL_buf = leaveFunRNL;
        RNL* rootFunRNL_buf = leaveFunRNL;
        ELTH_cur->ELSECOMPS = CompStates(&leaveFunRNL_buf, &rootFunRNL_buf);
        clear_RNL(rootFunRNL_buf);
    }
    else{
        strcpy(name_buf, token_name);
        ELTH_cur->ELSECOMPS = (COMPS*)malloc(sizeof(COMPS));
        ELTH_cur->ELSECOMPS = instantState(token_buf, name_buf, leaveFunRNL, rootFunRNL);
    }
    return ELTH_cur;
}


RTS* returnState(RNL* leaveFunRNL, RNL* rootFunRNL){
    RTS* RTS_cur = (RTS*)malloc(sizeof(RTS));
    char name_buf[MAX_TOKEN_SIZE];
    enum token_kind token_buf = get_token();
    strcpy(name_buf, token_name);
    //注意return不支持复合语句
    RTS_cur->NMS = Statement(token_buf, name_buf, leaveFunRNL, rootFunRNL);
    if(RTS_cur->NMS->res_kind != rootFunRNL->value_type){
        errorfound(RETERR);//wrong return type
    }
    return RTS_cur;
}

//复用了IFTH， 但删除了IF_STATUS部分
WHILETH* whileState(RNL* leaveFunRNL, RNL* rootFunRNL){
    WHILETH* WHILETH_cur = (WHILETH*)malloc(sizeof(WHILETH));
    enum token_kind token_buf = get_token();
    char name_buf[MAX_TOKEN_SIZE];
    if(token_buf == LP){
        token_buf = get_token();
        strcpy(name_buf, token_name);
        JUDGE_STATUS = 1;
        WHILETH_cur->NMS = Statement(token_buf, name_buf, leaveFunRNL, rootFunRNL);
        JUDGE_STATUS = 0;
        token_buf = get_token();
        if(token_buf == LB){
            RNL* leaveFunRNL_buf = leaveFunRNL;
            RNL* rootFunRNL_buf = leaveFunRNL;
            WHILETH_cur->WHILECOMPS = CompStates(&leaveFunRNL_buf, &rootFunRNL_buf);
            clear_RNL(rootFunRNL_buf);
        }
        else{
            strcpy(name_buf, token_name);
            WHILETH_cur->WHILECOMPS = (COMPS*)malloc(sizeof(COMPS));
            WHILETH_cur->WHILECOMPS = instantState(token_buf, name_buf, leaveFunRNL, rootFunRNL);
        }
    }
    else{
        errorfound(SYNERR);// invalid if statement
    }
    return WHILETH_cur;
}


//复用了IFTH， 但删除了IF_STATUS部分
FORTH* forState(RNL* leaveFunRNL, RNL* rootFunRNL){
    FORTH* FORTH_cur = (FORTH*)malloc(sizeof(FORTH));
    enum token_kind token_buf = get_token();
    char name_buf[MAX_TOKEN_SIZE];
    if(token_buf == LP){
        token_buf = get_token();
        strcpy(name_buf, token_name);
        FORTH_cur->NMS_start = Statement(token_buf, name_buf, leaveFunRNL, rootFunRNL);
        token_buf = get_token();
        strcpy(name_buf, token_name);
        FORTH_cur->NMS_check = Statement(token_buf, name_buf, leaveFunRNL, rootFunRNL);
        token_buf = get_token();
        strcpy(name_buf, token_name);
        JUDGE_STATUS = 1;
        FORTH_cur->NMS_end = Statement(token_buf, name_buf, leaveFunRNL, rootFunRNL);
        JUDGE_STATUS = 0;
        token_buf = get_token();
        if(token_buf == LB){
            RNL* leaveFunRNL_buf = leaveFunRNL;
            RNL* rootFunRNL_buf = leaveFunRNL;
            FORTH_cur->FORCOMPS = CompStates(&leaveFunRNL_buf, &rootFunRNL_buf);
            clear_RNL(rootFunRNL_buf);
        }
        else{
            strcpy(name_buf, token_name);
            FORTH_cur->FORCOMPS = (COMPS*)malloc(sizeof(COMPS));
            FORTH_cur->FORCOMPS = instantState(token_buf, name_buf, leaveFunRNL, rootFunRNL);
        }
    }
    else{
        errorfound(SYNERR);// invalid if statement
    }
    return FORTH_cur;
}



COMPS* instantState(enum token_kind token_buf ,char* name_buf, RNL* leaveFunRNL, RNL* rootFunRNL){
    COMPS* cur_COMPS = (COMPS*)malloc(sizeof(COMPS));
    if(token_buf == RET){
        cur_COMPS->COMP_kind = RETURNSTA;
        cur_COMPS->COMP = (COMP*)malloc(sizeof(COMP));
        int buf_if_status = IF_STATUS;
        IF_STATUS = 0;
        cur_COMPS->COMP->STA = (STA*)malloc(sizeof(STA));
        cur_COMPS->COMP->STA->RTS = returnState(leaveFunRNL, rootFunRNL);
        cur_COMPS->COMP->STA->WHILETH = NULL;
        cur_COMPS->COMP->STA->FORTH = NULL;
        cur_COMPS->COMP->STA->NMS = NULL;
        cur_COMPS->COMP->STA->ELTH = NULL;
        cur_COMPS->COMP->STA->IFTH = NULL;
        cur_COMPS->COMP->EVD = NULL;
        IF_STATUS = buf_if_status;
        cur_COMPS->next = (COMPS*)malloc(sizeof(COMPS));
        cur_COMPS->next->COMP_kind = ENDOFSTA;
    }
    else if(token_buf == IF){
        cur_COMPS->COMP_kind = IFSTA;
        cur_COMPS->COMP = (COMP*)malloc(sizeof(COMP));
        IF_STATUS = 0;
        cur_COMPS->COMP->STA = (STA*)malloc(sizeof(STA));
        cur_COMPS->COMP->STA->IFTH = ifState(leaveFunRNL, rootFunRNL);
        cur_COMPS->COMP->STA->WHILETH = NULL;
        cur_COMPS->COMP->STA->FORTH = NULL;
        cur_COMPS->COMP->STA->NMS = NULL;
        cur_COMPS->COMP->STA->ELTH = NULL;
        cur_COMPS->COMP->STA->RTS = NULL;
        cur_COMPS->COMP->EVD = NULL;
        IF_STATUS = 1;
        enum token_kind buf_token = get_token();
        if(buf_token == ELSE){
            cur_COMPS->next = (COMPS*)malloc(sizeof(COMPS));
            cur_COMPS->next->COMP_kind = ELSESTA;
            cur_COMPS->next->COMP =  (COMP*)malloc(sizeof(COMP));
            cur_COMPS->next->COMP->STA = (STA*)malloc(sizeof(STA));
            cur_COMPS->next->COMP->STA->ELTH = elseState(leaveFunRNL, rootFunRNL);
            cur_COMPS->next->COMP->STA->WHILETH = NULL;
            cur_COMPS->next->COMP->STA->FORTH = NULL;
            cur_COMPS->next->COMP->STA->NMS = NULL;
            cur_COMPS->next->COMP->STA->IFTH = NULL;
            cur_COMPS->next->COMP->STA->RTS = NULL;
            cur_COMPS->next->next = (COMPS*)malloc(sizeof(COMPS));
            cur_COMPS->next->next->COMP_kind = ENDOFSTA;
        }
        else{
            ungetc(' ', read_fp);
            ulong buf_len = strlen(token_name);
            ulong buf_index;
            for(buf_index = buf_len; buf_index > 0; buf_index--){
                ungetc(token_name[buf_index - 1], read_fp);
            }
            ungetc(' ', read_fp);
            cur_COMPS->next = (COMPS*)malloc(sizeof(COMPS));
            cur_COMPS->next->COMP_kind = ENDOFSTA;
        }
    }
    else if(token_buf == ELSE){
        if(IF_STATUS){
            IF_STATUS = 0;
            cur_COMPS->COMP_kind = ELSESTA;
            cur_COMPS->COMP = (COMP*)malloc(sizeof(COMP));
            cur_COMPS->COMP->STA = (STA*)malloc(sizeof(STA));
            cur_COMPS->COMP->STA->ELTH = elseState(leaveFunRNL, rootFunRNL);
            cur_COMPS->COMP->STA->WHILETH = NULL;
            cur_COMPS->COMP->STA->FORTH = NULL;
            cur_COMPS->COMP->STA->NMS = NULL;
            cur_COMPS->COMP->STA->IFTH = NULL;
            cur_COMPS->COMP->STA->RTS = NULL;
            cur_COMPS->COMP->EVD = NULL;
            cur_COMPS->next = (COMPS*)malloc(sizeof(COMPS));
            cur_COMPS->next->COMP_kind = ENDOFSTA;
        }
        else{
            errorfound(ELSEONLY);//else without if
        }
    }
    else if(token_buf == FOR){
        cur_COMPS = (COMPS*)malloc(sizeof(COMPS));
        cur_COMPS->COMP_kind = FORSTA;
        cur_COMPS->COMP = (COMP*)malloc(sizeof(COMP));
        int buf_if_status = IF_STATUS;
        IF_STATUS = 0;
        cur_COMPS->COMP->STA = (STA*)malloc(sizeof(STA));
        cur_COMPS->COMP->STA->FORTH = forState(leaveFunRNL, rootFunRNL);
        cur_COMPS->COMP->STA->WHILETH = NULL;
        cur_COMPS->COMP->STA->RTS = NULL;
        cur_COMPS->COMP->STA->NMS = NULL;
        cur_COMPS->COMP->STA->ELTH = NULL;
        cur_COMPS->COMP->STA->IFTH = NULL;
        cur_COMPS->COMP->EVD = NULL;
        IF_STATUS = buf_if_status;
        cur_COMPS->next = (COMPS*)malloc(sizeof(COMPS));
        cur_COMPS->next->COMP_kind = ENDOFSTA;
    }
    else if(token_buf == WHILE){
        cur_COMPS = (COMPS*)malloc(sizeof(COMPS));
        cur_COMPS->COMP_kind = WHILESTA;
        cur_COMPS->COMP = (COMP*)malloc(sizeof(COMP));
        int buf_if_status = IF_STATUS;
        IF_STATUS = 0;
        cur_COMPS->COMP->STA = (STA*)malloc(sizeof(STA));
        cur_COMPS->COMP->STA->WHILETH = whileState(leaveFunRNL, rootFunRNL);
        cur_COMPS->COMP->STA->RTS = NULL;
        cur_COMPS->COMP->STA->FORTH = NULL;
        cur_COMPS->COMP->STA->NMS = NULL;
        cur_COMPS->COMP->STA->ELTH = NULL;
        cur_COMPS->COMP->STA->IFTH = NULL;
        cur_COMPS->COMP->EVD = NULL;
        IF_STATUS = buf_if_status;
        cur_COMPS->next = (COMPS*)malloc(sizeof(COMPS));
        cur_COMPS->next->COMP_kind = ENDOFSTA;
    }
    else{
        cur_COMPS = (COMPS*)malloc(sizeof(COMPS));
        cur_COMPS->COMP_kind = NORMSTA;
        cur_COMPS->COMP = (COMP*)malloc(sizeof(COMP));
        int buf_if_status = IF_STATUS;
        IF_STATUS = 0;
        cur_COMPS->COMP->STA = (STA*)malloc(sizeof(STA));
        cur_COMPS->COMP->STA->NMS = Statement(token_buf, name_buf, leaveFunRNL, rootFunRNL);
        cur_COMPS->COMP->STA->WHILETH = NULL;
        cur_COMPS->COMP->STA->FORTH = NULL;
        cur_COMPS->COMP->STA->RTS = NULL;
        cur_COMPS->COMP->STA->ELTH = NULL;
        cur_COMPS->COMP->STA->IFTH = NULL;
        cur_COMPS->COMP->EVD = NULL;
        IF_STATUS = buf_if_status;
        cur_COMPS->next = (COMPS*)malloc(sizeof(COMPS));
        cur_COMPS->next->COMP_kind = ENDOFSTA;
    }
    return cur_COMPS;
}













