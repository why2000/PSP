//
//  reader.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#pragma once

//libraries
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//defines
#define MAXLINE 1024
#define MAX_TOKEN_SIZE 32
typedef unsigned long ulong;
typedef unsigned short ushort;
int anastart(const char*);

enum token_kind {
    EMPTY_TOKEN,
	ERROR_TOKEN,
    NUMBER,
    LETTER,
	IDENT,
	INT_CONST,
	FLOAT_CONST,
	CHAR_CONST,
	INT,
	FLOAT,
	CHAR,
	IF,
	ELSE,
    LB,// {
    RB,// }
    LP,// (
    RP,// )
    SEMI,// ;
    COMMA,// ,
    DOT,// .
    GEQ,// >=
    LEQ,// <=
    EQ,// ==
    NEQ,// !=
    PLUSEQ,// +=
    MINUSEQ,// -=
    MULTIEQ,// *=
    DIVIDEEQ,// /=
    LOR,// ||
    LAND,// &&
    NOR,// ^
    NOT,// !
    BOR,// |
    BAND,// &
    PLUS,// +
    MINUS,// -
    MULTI,// *
    DIVIDE,// /
	ASSIGN,// =
    GT,// >
    LT// <

};

enum struct_type {
    EXTDEFLIST,
    EXTVARNAMELIST,
    EXTVARDEF,
    FUNCDEF,
    STATELIST,
    IFTHEN,
    IFELSE,
    RETURNSTA,
    NORMSTA,
    ASSIGNSTA,
};
typedef struct ED{
    struct EVD* EVD;
    struct FUNCD* FUNCD;
}ED;
typedef struct COMP{
    //VarNameList
    struct EVD* EVD;
    //STALIST;
    struct STAL* STAL;
}COMP;
//ExtDefList
typedef struct EDL{
    //如果void指针不好用可以直接列出两个子树，必有一个为空,也可以union
    //ED_kind could be EXTVARDEF or FUNCDEF
    enum struct_type ED_kind;
    struct ED* ED;
    struct EDL* EDL;
}EDL;

//ExtVarNameList(int `a, b`)
typedef struct EVNL{
    char var_name[MAX_TOKEN_SIZE];
    struct EVNL* next;
}EVNL;

//ExtVarDef(`int a, b;`)
typedef struct EVD{
    //EVD_kind is a token kind.(int, char, etc.)
    enum token_kind EVD_kind;
    struct EVNL* EVNL;
}EVD;

//CompoundStatements
typedef struct COMPS{
    //同EDL理
    //COMP_kind could be EXTVARDEF or STATELIST
    enum struct_type COMP_kind;
    //CompState(sentence)(statement or LVL)
    struct COMP* COMP;
    //CompStates
    struct COMPS* COMPS;
}COMPS;

//AssignState(`a = b`)
typedef struct ASSSTA{
    void* ASSSTA;
}ASSSTA;

//constState(`1`)
typedef struct CONSTSTA{
    char* const_name;
}CONSTSTA;

//IDState(`a`)
typedef struct IDSTA{
    char* ID_name;
    //ActualParaList
    struct APL* APL;
}IDSTA;

//NormalState(`a > b`, `a + b`)
typedef struct NMSTA{
    //normalstate kind(>, <, >=, etc.)
    enum token_kind NM_kind;
    //leftSTA
    void* left;
    //rightSTA
    void* right;
}NMSTA;


//if-then-else
typedef struct ELIF{
    struct NMSTA* NMSTA;
    struct COMPS* IFCOMPS;
    struct COMPS* ELCOMPS;
}ELIF;

//if-then
typedef struct THIF{
    struct NMSTA* NMSTA;
    struct COMPS* IFCOMPS;
}THIF;

//return
typedef struct RETSTA{
    enum struct_type STA_kind;
    void* STA;
}RETSTA;

//ActualParaList
typedef struct APL{
    enum struct_type STA_kind;
    //Statement
    void* STA;
    //ActualParaList
    struct APL* APL;
}APL;

//funcState
typedef struct FUNCSTA{
    enum struct_type STA_kind;
    //FuncName
    char func_name[MAX_TOKEN_SIZE];
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


//FormParaList
//Notice that "int fun(float a,b);" is not a proper type, "b" will be of "int".
typedef struct FPL{
    //FPKind is a token kind
    enum token_kind FP_kind;
    //FormParaName
    char FP_name[MAX_TOKEN_SIZE];
    //FormParaList
    struct FPL* next;
}FPL;

//funcDef
typedef struct FUNCD{
    //funcKind is a token kind
    enum token_kind FUNC_kind;
    //funcName
    char FUNC_name[MAX_TOKEN_SIZE];
    //FormalParaList
    struct FPL* FPL;
    //funcBody
    struct COMPS* FUNCB;
}FUNCD;

//includes
#include"utils.h"
#include"ExtVarDef.h"
#include"ExtDef.h"
#include"output.h"
#include"errors.h"






















