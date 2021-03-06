//
//  reader.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#pragma once

#include"main.h"
////remove left spaces, return in heap
//char* lstrip(const char*);
////remove right spaces, return in heap
//char* rstrip(const char*);
////remove spaces, return in heap
//char* strip(const char*);
//read one token from file
enum token_kind get_token(void);
enum token_kind single_token(char);
void to_string(enum token_kind, char*);
int is_number(char);
int is_letter(char);
int check_single(enum token_kind);
int check_oneside(enum token_kind);
int check_special(enum token_kind);
int check_bothside(enum token_kind);
int check_const(enum token_kind);
int check_bool(enum token_kind);
int check_declare(enum token_kind);
int check_operator(enum token_kind);
int order_operator(enum token_kind);
int compare_operator(enum token_kind, enum token_kind);
void push_RNL(RNL**, char*, enum struct_type, enum token_kind);
enum token_kind check_registed(RNL*, char*, enum struct_type, APL*);
int search_RNL(RNL*, char*);
void clear_RNL(RNL*);
