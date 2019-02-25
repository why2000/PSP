//
//  StateList.h
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/22.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#pragma once

#include "main.h"

//repol
typedef struct REPOL{
    enum token_kind token_kind;
    enum token_kind res_kind;
    char token_name[MAX_TOKEN_SIZE];
    struct APL* APL;
    struct REPOL* next;
}REPOL;

//op stack
typedef struct SOP{
    enum token_kind op_kind;
    struct SOP* next;
    struct SOP* last;
}SOP;

NMS* Statement(enum token_kind, char*, RNL*, RNL*);
NMS* geneNMS(REPOL**, NMS**);
APL* funCall(enum token_kind, char*, RNL*, RNL*);
IFTH* ifState(RNL*, RNL*);
WHILETH* whileState(RNL*, RNL*);
ELTH* elseState(RNL*, RNL*);
FORTH* forState(RNL*, RNL*);
RTS* returnState(RNL*, RNL*);
COMPS* instantState(enum token_kind, char*, RNL*, RNL*);



void push_stack(SOP**, enum token_kind);
enum token_kind get_stack(SOP**);
enum token_kind pop_stack(SOP**);
void push_list(REPOL** repol, enum token_kind token_kind, char* token_name);
NMS* pop_out(NMS**);
void push_out(NMS**, REPOL*);
