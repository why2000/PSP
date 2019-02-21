//
//  reader.h
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

//未考虑的点:
//下划线标识符
//转义字符


#include"utils.h"

extern int line_num;//"main.c"
//clear everytime used
extern char token_name[MAX_TOKEN_SIZE];//"main.c"

//remove left spaces, return in heap
char* lstrip(const char* oristr) {
	//heap
	char* newstr = (char*)malloc(strlen(oristr)+1);
	char* orip;
	char* newp = newstr;
	for (orip = (char*)oristr; *orip == ' '; orip++);
	for (; *orip != '\0'; orip++, newp++) {
		*newp = *orip;
	}
	*newp = '\0';
	return newstr;
}

//remove right spaces, return in heap
char* rstrip(const char* oristr) {
	//heap
	char* newstr = (char*)malloc(strlen(oristr)+1);
	ulong orip;
	ulong newp;
	ulong length = strlen(oristr);
	for (orip = length - 1, newp = length - 1; oristr[orip] == ' '; orip--, newp--);
	newstr[newp + 1] = '\0';
	for (; orip+1 > 0; orip--, newp--) {
		newstr[newp] = oristr[orip];
	}
	return newstr;
}

//remove spaces, return in heap
char* strip(const char* oristr) {
	//heap
	char* newstr = (char*)malloc(strlen(oristr)+1);
	//heap
	char* lbufstr = (char*)malloc(strlen(oristr)+1);
	lbufstr = lstrip(oristr);
	newstr = rstrip(lbufstr);
	free(lbufstr);
	return newstr;
}

enum token_kind get_token(FILE* read_fp){
    char buf_ch = '\0';
    
    ushort index;
    
    //remove spaces
    while(buf_ch == '\0' || buf_ch == ' ' || buf_ch  == '\t' || buf_ch == '\n'){
        if(buf_ch == '\n') line_num += 1; 
        buf_ch = fgetc(read_fp);
    };
    ungetc(buf_ch ,read_fp);
    
    //init
    enum token_kind last_token = EMPTY_TOKEN;
    enum token_kind cur_token = EMPTY_TOKEN;
    
    
    for(index = 0;((buf_ch = fgetc(read_fp))!= EOF);index++){
        if(buf_ch == ' ' || buf_ch  == '\t' || buf_ch == '\n'){
            if(buf_ch == '\n') line_num += 1;
            return cur_token;
        }
        token_name[index] = buf_ch;
        cur_token = single_token(buf_ch);
        if(last_token == CHAR_CONST){
            if(token_name[index-1] != '\'' && buf_ch != '\''){
                errorfound(0);//string not supported
            }
            if(buf_ch == '\''){
                errorfound(0);//empty char
            }
        }
        //limited cur to single token or empty
        if(check_single(cur_token)){
            //limited cur and last to single
            if(check_single(last_token)){
                switch(cur_token){
                    case ASSIGN:
                        switch(last_token){
                            case GT:
                                token_name[index+1] = '\0';
                                return GEQ;// >=
                            case LT:
                                token_name[index+1] = '\0';
                                return LEQ;// <=
                            case ASSIGN:
                                token_name[index+1] = '\0';
                                return EQ;// ==
                            case NOT:
                                token_name[index+1] = '\0';
                                return NEQ;// !=
                            case PLUS:
                                token_name[index+1] = '\0';
                                return PLUSEQ;// +=
                            case MINUS:
                                token_name[index+1] = '\0';
                                return MINUSEQ;// -=
                            case MULTI:
                                token_name[index+1] = '\0';
                                return MULTIEQ;// *=
                            case DIVIDE:
                                token_name[index+1] = '\0';
                                return DIVIDEEQ;// /=
                            default:
                                errorfound(0);//invalid operator
                        }
                    default:
                        errorfound(0);//invalid operator
                }
            }
            else if(last_token == EMPTY_TOKEN){
                //nothing to do
                //cur_token is already assgined
                ;
            }
            else if(last_token == CHAR_CONST){
                cur_token = CHAR_CONST;
            }
            else{
                token_name[index] = '\0';
                ungetc(buf_ch, read_fp);
                return last_token;
            }
        }
        else if(is_letter(buf_ch)){
            if(last_token == IDENT || last_token == INT || last_token == FLOAT || last_token == CHAR){
                cur_token = IDENT;
                if(buf_ch == 't'){
                    if(index == 2){
                        if(token_name[0] == 'i' && token_name[1] == 'n'){
                            cur_token = INT;
                        }
                    }
                    if(index == 4){
                        if(token_name[0] == 'f' && token_name[1] == 'l' && token_name[2] == 'o' && token_name[3] == 'a'){
                            cur_token = FLOAT;
                        }
                    }
                }
                if(buf_ch == 'r'){
                    if(index == 3){
                        if(token_name[0] == 'c' && token_name[1] == 'h' && token_name[2] == 'a'){
                            cur_token = CHAR;
                        }
                    }
                }
            }
            else if(last_token == CHAR_CONST){
               cur_token = CHAR_CONST;
            }
            else if(last_token == EMPTY_TOKEN){
                cur_token = IDENT;
            }
            else{
                token_name[index] = '\0';
                ungetc(buf_ch, read_fp);
                return last_token;
            }
        }
        else if(is_number(buf_ch)){
            if(last_token == INT_CONST || last_token == FLOAT_CONST || last_token == CHAR_CONST){
                cur_token = last_token;
            }
            else if (last_token == IDENT || last_token == INT || last_token == FLOAT || last_token == CHAR){
                cur_token = IDENT;
            }
            else if (last_token == EMPTY_TOKEN){
                cur_token = INT_CONST;
            }
            else{
                token_name[index] = '\0';
                ungetc(buf_ch, read_fp);
                return last_token;
            }
        }
        else if(buf_ch == '\''){
            if(last_token == CHAR_CONST){
                if(token_name[index-1] == '\''){
                    errorfound(0);//empty char
                }
                else{
                    //char should be returned in time
                    //to avoid unclosed char
                    cur_token = CHAR_CONST;
                    token_name[index+1] = '\0';
                    return cur_token;
                }
            }
            else if(last_token == EMPTY_TOKEN){
                cur_token = CHAR_CONST;
            }
            else{
                token_name[index] = '\0';
                ungetc(buf_ch, read_fp);
                return last_token;
            }
        }
        else if(buf_ch == ','){
            if(last_token == EMPTY_TOKEN){
                cur_token = COMMA;
                token_name[index+1] = '\0';
                return cur_token;
            }
            else{
                token_name[index] = '\0';
                ungetc(buf_ch, read_fp);
                return last_token;
            }
        }
        else if(buf_ch == '.'){
            if(last_token == INT_CONST){
                cur_token = FLOAT_CONST;
            }
            else if(last_token == FLOAT_CONST){
                errorfound(0);//invalid float
            }
            else if(last_token == EMPTY_TOKEN){
                cur_token = DOT;
                token_name[index+1] = '\0';
                return cur_token;
            }
            else{
                token_name[index] = '\0';
                ungetc(buf_ch, read_fp);
                return last_token;
            }
        }
        else if(buf_ch == ';'){
            if(last_token == EMPTY_TOKEN){
                cur_token = SEMI;
                token_name[index+1] = '\0';
                return cur_token;
            }else{
                token_name[index] = '\0';
                ungetc(buf_ch, read_fp);
                return last_token;
            }
        }
        else if(buf_ch == '('){
            if(last_token == EMPTY_TOKEN){
                cur_token = LP;
                token_name[index+1] = '\0';
                return cur_token;
            }else{
                token_name[index] = '\0';
                ungetc(buf_ch, read_fp);
                return last_token;
            }
        }
        else if(buf_ch == ')'){
            if(last_token == EMPTY_TOKEN){
                cur_token = RP;
                token_name[index+1] = '\0';
                return cur_token;
            }else{
                token_name[index] = '\0';
                ungetc(buf_ch, read_fp);
                return last_token;
            }
        }
        else if(buf_ch == '{'){
            if(last_token == EMPTY_TOKEN){
                cur_token = LB;
                token_name[index+1] = '\0';
                return cur_token;
            }else{
                token_name[index] = '\0';
                ungetc(buf_ch, read_fp);
                return last_token;
            }
        }
        else if(buf_ch == '}'){
            if(last_token == EMPTY_TOKEN){
                cur_token = RB;
                token_name[index+1] = '\0';
                return cur_token;
            }else{
                token_name[index] = '\0';
                ungetc(buf_ch, read_fp);
                return last_token;
            }
        }
        token_name[index+1] = '\0';
        last_token = cur_token;
    }
    return cur_token;
}



//return single token kind
enum token_kind single_token(char buf_ch){
    switch(buf_ch){
        case '!':
            return NOT;
        case '+':
            return PLUS;
        case '-':
            return MINUS;
        case '*':
            return MULTI;
        case '/':
            return DIVIDE;
        case '=':
            return ASSIGN;
        case '>':
            return GT;
        case '<':
            return LT;
        case ';':
            return SEMI;
        case ',':
            return COMMA;
        case '(':
            return LP;
        case ')':
            return RP;
        case '{':
            return LB;
        case '}':
            return RB;
        case '|':
            return BOR;
        case '&':
            return BAND;
        default:
            return EMPTY_TOKEN;
    }
    return EMPTY_TOKEN;
}


int is_letter(char buf_ch){
    if((buf_ch >= 'a' && buf_ch <= 'z') || (buf_ch >= 'A' && buf_ch <= 'Z'))return 1;
    return 0;
}


int is_number(char buf_ch){
    if((buf_ch >= '0' && buf_ch <= '9'))return 1;
    return 0;
}

//check whether token is single
int check_single(enum token_kind tk){
    if (tk >= NOT) return 1;
    return 0;
}

int check_const(enum token_kind tk){
    if (tk == INT_CONST || tk == FLOAT_CONST || tk == CHAR_CONST)return 1;
    return 0;
}

int check_declare(enum token_kind tk){
    if (tk == INT || tk == FLOAT || tk == CHAR)return 1;
    return 0;
}
    












