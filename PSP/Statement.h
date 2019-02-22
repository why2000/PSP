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
    char token_name[MAX_TOKEN_SIZE];
    struct REPOL* next;
}REPOL;

//op stack
typedef struct SOP{
    enum token_kind op_kind;
    struct SOP* next;
    struct SOP* last;
}SOP;

NMS* Statement(enum token_kind, char*);
IFTH* ifState(RNL*, RNL*);
ELTH* elseState(RNL*, RNL*);
RTS* returnState(void);



void push_stack(SOP**, enum token_kind);
enum token_kind get_stack(SOP**);
enum token_kind pop_stack(SOP**);
void push_list(REPOL** repol, enum token_kind token_kind, char* token_name);

