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

//defines
#define MAXLINE 1024
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

