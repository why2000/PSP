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
#define MAX_TOKEN_NUM 1024
#define MAX_TOKEN_SIZE 32
typedef unsigned long ulong;
typedef unsigned short ushort;
int anastart(const char*);

enum token_kind {
    EMPTY_TOKEN = 0,
	ERROR_TOKEN,
    FUNC,
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
    RET,
    WHILE,
    FOR,
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
    MODEQ,// %=
    MULTIEQ,// *=
    DIVIDEEQ,// /=
    LOR,// ||
    LAND,// &&
    XOR,// ^
    NOT,// !
    MOD,// %
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
    IFSTA,
    ELSESTA,
    RETURNSTA,
    WHILESTA,
    FORSTA,
    NORMSTA,
    FUNCSTA,
    ENDOFSTA,
    STARTOFSTA
};

//每个函数解析过程中传入当前外部注册表与函数内部注册表
//RegisteredNameList
typedef struct RNL{
    char var_name[MAX_TOKEN_SIZE];
    enum token_kind value_type;
    //FUNCDEF or EXTVARDEF
    enum struct_type var_type;
    struct FPL* FPL;
    struct RNL* next;
    struct RNL* last;
}RNL;

//StateList
typedef struct STA{
    struct FUNS* FUNS;
    struct RTS* RTS;
    struct IFTH* IFTH;
    struct ELTH* ELTH;
    struct NMS* NMS;
    struct WHILETH* WHILETH;
    // need change
    struct FORTH* FORTH;
}STA;

//ExtDef
typedef struct ED{
    struct EVD* EVD;
    struct FUNCD* FUNCD;
}ED;
typedef struct COMP{
    //VarNameList
    struct EVD* EVD;
    //STALIST;
    struct STA* STA;
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
    //next CompState
    struct COMPS* next;
}COMPS;

//NormalState(`a > b`, `a + b`)
typedef struct NMS{
    //normalstate kind(>, <, >=, etc.)
    enum token_kind NM_kind;
    //result kind(int, float, const)
    enum token_kind res_kind;
    //name of var/const
    char NMS_name[MAX_TOKEN_SIZE];
    struct APL* APL;
    //leftSTA, NULL if const/var
    struct NMS* left;
    //rightSTA, NULL if const/var
    struct NMS* right;
    struct NMS* next;
    struct NMS* last;
}NMS;


//if-then(else is comps)
typedef struct IFTH{
    struct NMS* NMS;
    struct COMPS* IFCOMPS;
}IFTH;

typedef struct WHILETH{
    struct NMS* NMS;
    struct COMPS* WHILECOMPS;
}WHILETH;

typedef struct FORTH{
    struct NMS* NMS_start;
    struct NMS* NMS_check;
    struct NMS* NMS_end;
    struct COMPS* FORCOMPS;
}FORTH;

//else-then
typedef struct ELTH{
    struct COMPS* ELSECOMPS;
}ELTH;

//return
typedef struct RTS{
    struct NMS* NMS;
}RTS;

//ActualParaList
typedef struct APL{
    //Statement
    NMS* NMS;
    //next ActualParaList
    struct APL* next;
}APL;

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
#include"ExtDef.h"
#include"ExtVarDef.h"
#include"FuncDef.h"
#include"COMPS.h"
#include"Statement.h"
#include"output.h"
#include"errors.h"






















