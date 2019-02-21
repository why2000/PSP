//
//  reader.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#pragma once

#include"main.h"
//remove left spaces, return in heap
char* lstrip(const char*);
//remove right spaces, return in heap
char* rstrip(const char*);
//remove spaces, return in heap
char* strip(const char*);
//read one token from file
enum token_kind get_token(FILE*);
enum token_kind single_token(char);
int is_number(char buf_ch);
int is_letter(char buf_ch);
int check_single(enum token_kind);
int check_const(enum token_kind);
int check_declare(enum token_kind);
