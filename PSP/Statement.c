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
NMS* Statement(enum token_kind first_token, char* first_name){
    NMS* NMS_cur = (NMS*)malloc(sizeof(NMS));
    enum token_kind token_buf = first_token;
    REPOL* repol = (REPOL*)malloc(sizeof(REPOL));
    REPOL* repol_root = repol;
    SOP* sop = (SOP*)malloc(sizeof(SOP));
    SOP* sop_root = sop;
    sop->last = NULL;
    int now = 0;//input读取符
    int top = -1;//in栈顶符,读取top为负时表示出现错误
    int pol = 0;//repol栈写入符
    while (token_buf != SEMI) {
        //1.括号处理
        if (token_buf == LP) {//左括号无视优先级直接入栈
            push_stack(&sop, token_buf);
            token_buf = get_token();
        }
        else if (token_buf == RP) {//注意右括号本身永远不会存在于入栈中，后续不用判断其存在
            //左括号上方符号出栈
            while ((get_stack(&sop) != LP) && (sop != NULL)) {
                push_list(&repol, pop_stack(&sop), NULL);
            }
            //*************************错误1：右括号多于左括号********************************
            if (sop == NULL) {
                if(IF_STATUS){
                    break;
                }
                else{
                    errorfound(1);
                }
            }
            //********************************ERROR1**************************************
            sop = sop->last;//舍弃左括号
            //舍弃右括号
            token_buf = get_token();
        }
        //2.读取运算符
        else if (token_buf=='@'||token_buf=='^') {//最高级运算符@与^（开方/乘方）同级运算符及其上方的全部运算符出栈压入repol，再将本身压入入栈
            while (sop != NULL && (get_stack(&sop) == '@' || get_stack(&sop) == '^')) {
                push_list(&repol, pop_stack(&sop), NULL);
            }
            push_stack(&sop, token_buf);
            token_buf = get_token();
        }
        else if (token_buf == '*' || token_buf == '/') {//次高级运算符*与/将同级或高级运算符出栈压入repol,再将本身压入入栈
            while (sop != NULL && (get_stack(&sop) == '@' || get_stack(&sop) == '^' || get_stack(&sop) == '*' || get_stack(&sop) == '/')) {
                push_list(&repol, pop_stack(&sop), NULL);
            }
            push_stack(&sop, token_buf);
            token_buf = get_token();
        }
        else if (token_buf == '+' || token_buf == '-') {//低级运算符+与-将左括号上方的全部运算符出栈压入repol,再将本身压入入栈
            while (sop!= NULL && get_stack(&sop) != LP) {
                push_list(&repol, pop_stack(&sop), NULL);
            }
            push_stack(&sop, token_buf);
            token_buf = get_token();
        }
        else if((input[now] >= '0'&&input[now] <= '9') || (input[now] == '.')) {//数字（含浮点）直接写入逆波兰栈
            while ((input[now] >= '0'&&input[now] <= '9')||input[now]=='.') {
                repol[pol++] = input[now++];
            }
            repol[pol++] = ' ';//数字读入结束标识，防止repol中压入连续数字产生混淆
        }
        //**************************错误6：无法识别的输入**************************************
        else {
            errorfound(6);
        }
        //**********************************ERROR6*****************************************
    }
    while (top >= 0) {//符号压入repol
        //***********************错误2：左括号多于右括号************************************
        if (stack[top] == '(') {
            repol[pol++]='\0';//结束逆波兰栈的写入，并返回0以请求继续输入
            errorfound(2);
        }
        //******************************ERROR2******************************************
        repol[pol++] = stack[top--];
    }
    repol[pol++] = '\0';//逆波兰栈写入完成
    return NMS_cur;
}

void push_stack(SOP** sop, enum token_kind op_kind){
    SOP* buf_sop = (*sop);
    (*sop)->next = (SOP*)malloc(sizeof(SOP));
    (*sop) = (*sop)->next;
    (*sop)->last = buf_sop;
    (*sop)->op_kind = op_kind;
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

void push_list(REPOL** repol, enum token_kind token_kind, char* token_name){
    (*repol)->token_kind = token_kind;
    strcpy((*repol)->token_name, token_name);
    (*repol)->next = (REPOL*)malloc(sizeof(REPOL));
    (*repol) = (*repol)->next;
}


IFTH* ifState(RNL* leaveFunRNL, RNL* rootFunRNL){
    IFTH* IFTH_cur = (IFTH*)malloc(sizeof(IFTH));
    enum token_kind token_buf = get_token();
    char name_buf[MAX_TOKEN_SIZE];
    if(token_buf == LP){
        token_buf = get_token();
        strcpy(name_buf, token_name);
        IFTH_cur->NMS = Statement(token_buf, name_buf);
        token_buf = get_token();
        if(token_buf == RB){
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
            IFTH_cur->IFCOMPS->COMP->STA->NMS = Statement(token_buf, name_buf);
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
    if(token_buf == RB){
        RNL* leaveFunRNL_buf = leaveFunRNL;
        RNL* rootFunRNL_buf = rootFunRNL;
        ELTH_cur->ELSECOMPS = CompStates(&leaveFunRNL_buf, &rootFunRNL_buf);
    }
    else{
        strcpy(name_buf, token_name);
        ELTH_cur->ELSECOMPS = (COMPS*)malloc(sizeof(COMPS));
        ELTH_cur->ELSECOMPS->COMP_kind = NORMSTA;
        ELTH_cur->ELSECOMPS->COMP = (COMP*)malloc(sizeof(COMP));
        ELTH_cur->ELSECOMPS->COMP->STA->NMS = Statement(token_buf, name_buf);
        ELTH_cur->ELSECOMPS->next = NULL;
    }
    return ELTH_cur;
}


RTS* returnState(void){
    RTS* RTS_cur = (RTS*)malloc(sizeof(RTS));
    char name_buf[MAX_TOKEN_SIZE];
    enum token_kind token_buf = get_token();
    strcpy(name_buf, token_name);
    RTS_cur->NMS = Statement(token_buf, name_buf);
    return RTS_cur;
}










