#pragma once

//libraries
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


//includes
#include<utils.h>

//defines
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

