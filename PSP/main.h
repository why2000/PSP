//
//  reader.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/17.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#pragma once

//libraries
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


//includes
#include"utils.h"
#include "ExtDefList.h"

//defines
#define MAXLINE 1024
typedef unsigned long ulong;

enum token_kind {
	ERROR_TOKEN,
	IDENT,
	INT_CONST,
	FLOAT_CONST,
	CHAR_CONST,
	INT,
	FLOAT,
	CHAR,
	IF,
	ELSE,
	EQ,
	ASSIGN,
	LP,
	RP,
	SEMI,
	COMMA
};

enum struct_type {
    EXTDEFLIST,
    EXTVARNAMELIST,
    EXTVARDEF,
    FUNCDEF,
    STATELIST,
    IFTHEN,
    IFELSE,
    RETURNSTA
};

//ExtDefList
typedef struct EDL{
    //如果void指针不好用可以直接列出两个子树，必有一个为空
    //ED_kind could be EXTVARDEF or FUNCDEF
    enum struct_type ED_kind;
    void* ED;
    struct EDL* EDL;
}EDL;

//ExtVarNameList
typedef struct EVNL{
    char* var_name;
    struct ENVL* next;
}EVNL;

//ExtVarDef
typedef struct EVD{
    //EVD_kind is a token kind.(int, char, etc.)
    enum token_kind EVD_kind;
    struct ENVL* ENVL;
}EVD;

//if-then-else
typedef struct ELIF{
    
}ELIF;

//if-then
typedef struct THIF{
    
}THIF;

//return
typedef struct RETSTA{
    void* STA;
}RETSTA;

//ActualParaList
typedef struct APL{
    //Statement
    void* STA;
    //
    struct APL* APL;
}APL;

//funcState
typedef struct FUNCSTA{
    //FuncName
    char* func_name;
    //ActualParaList
    struct APL* APL;
}FUNCSTA;

//StateList
typedef struct STAL{
    //STA_kind could be IFELSE, IFTHEN, NORMSTA, RETSTA
    enum struct_type STA_kind;
    //Statement
    void* STA;
    //StateList
    struct STAL* STAL;
}STAL;

//CompoundStatements
typedef struct COMPS{
    //同EDL理
    //COMP_kind could be EXTVARNAMELIST or STATELIST
    enum struct_type COMP_kind;
    //CompState(sentence)(statement or LVL)
    void* COMP;
    //CompStates
    struct COMPS* COMPS;
}COMPS;

//FormParaList
//Notice that "int fun(float a,b);" is not a proper type, "b" will be of "int".
typedef struct FPL{
    //FormParaKind is a token kind
    enum token_kind FP_kind;
    //FormParaName
    char* FP_name;
    //FormParaList
    struct FPL* next;
}FPL;

//funcDef
typedef struct FUNCD{
    //funcKind is a token kind
    enum token_kind FUNC_kind;
    //FormalParaList
    struct FPL* FPL;
    //funcBody
    struct COMPS* FUNCB;
}FUNCD;
























