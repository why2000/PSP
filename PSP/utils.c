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
extern FILE* read_fp;//"main.c"
extern RNL* rootRNL;//"main.c"
extern RNL* leaveRNL;//"main.c"
extern ANNOTATIONS* Annotations;
extern PRECOMPILES* Precompiles;


//写了再说，谁知道用不用得上
////remove left spaces, return in heap
//char* lstrip(const char* oristr) {
//    //heap
//    char* newstr = (char*)malloc(strlen(oristr)+1);
//    char* orip;
//    char* newp = newstr;
//    for (orip = (char*)oristr; *orip == ' '; orip++);
//    for (; *orip != '\0'; orip++, newp++) {
//        *newp = *orip;
//    }
//    *newp = '\0';
//    return newstr;
//}
//
////remove right spaces, return in heap
//char* rstrip(const char* oristr) {
//    //heap
//    char* newstr = (char*)malloc(strlen(oristr)+1);
//    ulong orip;
//    ulong newp;
//    ulong length = strlen(oristr);
//    for (orip = length - 1, newp = length - 1; oristr[orip] == ' '; orip--, newp--);
//    newstr[newp + 1] = '\0';
//    for (; orip+1 > 0; orip--, newp--) {
//        newstr[newp] = oristr[orip];
//    }
//    return newstr;
//}
//
////remove spaces, return in heap
//char* strip(const char* oristr) {
//    //heap
//    char* newstr = (char*)malloc(strlen(oristr)+1);
//    //heap
//    char* lbufstr = (char*)malloc(strlen(oristr)+1);
//    lbufstr = lstrip(oristr);
//    newstr = rstrip(lbufstr);
//    free(lbufstr);
//    return newstr;
//}

enum token_kind get_token(void){
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
                errorfound(SYNERR);//string not supported
            }
            if(buf_ch == '\''){
                errorfound(SYNERR);//empty char
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
                            case MOD:
                                token_name[index+1] = '\0';
                                return MODEQ;// %=
                            case MULTI:
                                token_name[index+1] = '\0';
                                return MULTIEQ;// *=
                            case DIVIDE:
                                token_name[index+1] = '\0';
                                return DIVIDEEQ;// /=
                            default:
                                errorfound(SYNERR);//invalid operator
                        }
                    case DIVIDE:
                        if(last_token == DIVIDE){
                            int ANN_index;
                            int ANN_total = 0;
                            while(1){
                                char ANN_bufc = fgetc(read_fp);
                                if(ANN_bufc == '\n'){
                                    break;
                                }
                                ANN_total++;
                            }
                            fseek(read_fp, -(ANN_total+1), SEEK_CUR);
                            Annotations->ANN_type = SINGLEANN;
                            Annotations->ANN_string = (char*)malloc(sizeof(ANN_total));
                            Annotations->line = line_num;
                            for(ANN_index = 0; ANN_index < ANN_total; ANN_index++){
                                char ann_bufc = fgetc(read_fp);
                                Annotations->ANN_string[ANN_index] = ann_bufc;
                            }
                            Annotations->ANN_string[ANN_index] = '\0';
                            Annotations->next = (ANNOTATIONS*)malloc(sizeof(ANNOTATIONS));
                            Annotations = Annotations->next;
                            Annotations->next = NULL;
                            return get_token();
                        }
                    case PLUS:
                        if(last_token == PLUS){
                            return DBPLUS;
                        }
                        else{
                            errorfound(SYNERR);
                        }
                    case MINUS:
                        if(last_token == MINUS){
                            return DBMINUS;
                        }
                        else{
                            errorfound(SYNERR);
                        }
                    case MULTI:
                        if(last_token == DIVIDE){
                            int ANN_index;
                            int ANN_total = 0;
                            while(1){
                                char ANN_bufc = fgetc(read_fp);
                                if(ANN_bufc == '*'){
                                    if((ANN_bufc = fgetc(read_fp)) == '/'){
                                        ungetc('/', read_fp);
                                        break;  
                                    }
                                }
                                ANN_total++;
                            }
                            fseek(read_fp, -(ANN_total+1), SEEK_CUR);
                            Annotations->ANN_type = MULTIANN;
                            Annotations->ANN_string = (char*)malloc(sizeof(ANN_total));
                            Annotations->line = line_num;
                            for(ANN_index = 0; ANN_index < ANN_total; ANN_index++){
                                char ann_bufc = fgetc(read_fp);
                                Annotations->ANN_string[ANN_index] = ann_bufc;
                            }
                            Annotations->ANN_string[ANN_index] = '\0';
                            fgetc(read_fp);
                            fgetc(read_fp);
                            Annotations->next = (ANNOTATIONS*)malloc(sizeof(ANNOTATIONS));
                            Annotations = Annotations->next;
                            Annotations->next = NULL;
                            return get_token();
                        }
                        else{
                            errorfound(SYNERR);
                        }
                    case GT:
                        if(last_token == MINUS){
                            token_name[index+1] = '\0';
                            return CHOOSE;// ->
                        }
                        else{
                            errorfound(SYNERR);
                        }
                    default:
                        errorfound(SYNERR);//invalid operator
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
            if(last_token == IDENT || last_token == INT || last_token == FLOAT || last_token == CHAR || last_token == IF || last_token == ELSE || last_token == RET || last_token == WHILE || last_token == FOR){
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
                    else if(index == 2){
                        if(token_name[0] == 'f' && token_name[1] == 'o'){
                            cur_token = FOR;
                        }
                    }
                }
                if(buf_ch == 'e'){
                    if(index == 3){
                        if(token_name[0] == 'e' && token_name[1] == 'l' && token_name[2] == 's'){
                            cur_token = ELSE;
                        }
                    }
                    else if(index == 4){
                        if(token_name[0] == 'w' && token_name[1] == 'h' && token_name[2] == 'i' && token_name[3] == 'l'){
                            cur_token = WHILE;
                        }
                    }
                }
                if(buf_ch == 'n'){
                    if(index == 5){
                        if(token_name[0] == 'r' && token_name[1] == 'e' && token_name[2] == 't' && token_name[3] == 'u' && token_name[4] == 'r'){
                            cur_token = RET;
                        }
                    }
                }
                if(buf_ch == 'f'){
                    if(index == 1){
                        if(token_name[0] == 'i'){
                            cur_token = IF;
                        }
                    }
                }
            }
            else if(last_token == NOT){
                errorfound(INVALID);//operator ! not supported
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
            else if (last_token == IDENT || last_token == INT || last_token == FLOAT || last_token == CHAR || last_token == CHAR || last_token == IF || last_token == ELSE || last_token == RET || last_token == WHILE || last_token == FOR){
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
                    errorfound(SYNERR);//empty char
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
                errorfound(SYNERR);//invalid float
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
        else if(buf_ch == '#'){
            Precompiles->line = line_num;
            char PC_bufc = fgetc(read_fp);
            if(PC_bufc == 'i'){
                if(fgetc(read_fp) == 'n'){
                    if(fgetc(read_fp) == 'c'){
                        if(fgetc(read_fp) == 'l'){
                            if(fgetc(read_fp) == 'u'){
                                if(fgetc(read_fp) == 'd'){
                                    if(fgetc(read_fp) == 'e'){
                                        while((PC_bufc = fgetc(read_fp)) == ' ' || PC_bufc == '\t'){
                                            ;
                                        }
                                        Precompiles->PC_type = INCLUDE;
                                        if(PC_bufc == '<'){
                                            int PC_index;
                                            for(PC_index = 0; 1; PC_index++){
                                                PC_bufc = fgetc(read_fp);
                                                if(PC_bufc == '>'){
                                                    break;
                                                }
                                                else if(PC_bufc == '\n'){
                                                    errorfound(SYNERR);
                                                    break;
                                                }
                                                else{
                                                    Precompiles->firstname[PC_index] = PC_bufc;
                                                }
                                            }
                                            Precompiles->firstname[PC_index] = '\0';
                                        }
                                        else if(PC_bufc == '\"'){
                                            int PC_index;
                                            for(PC_index = 0; 1; PC_index++){
                                                PC_bufc = fgetc(read_fp);
                                                if(PC_bufc == '\"'){
                                                    break;
                                                }
                                                else if(PC_bufc == '\n'){
                                                    errorfound(SYNERR);
                                                    break;
                                                }
                                                else{
                                                    Precompiles->firstname[PC_index] = PC_bufc;
                                                }
                                            }
                                            Precompiles->firstname[PC_index] = '\0';
                                        }
                                        while((PC_bufc = fgetc(read_fp)) != '\n'){
                                            if(PC_bufc != ' ' && PC_bufc != '\t'){
                                                errorfound(SYNERR);
                                            }
                                        }
                                        ungetc('\n', read_fp);
                                    }
                                    else{
                                        errorfound(SYNERR);
                                    }
                                }
                                else{
                                    errorfound(SYNERR);
                                }
                            }
                            else{
                                errorfound(SYNERR);
                            }
                        }
                        else{
                            errorfound(SYNERR);
                        }
                    }
                    else{
                        errorfound(SYNERR);
                    }
                }
                else{
                    errorfound(SYNERR);
                }
            }
            else if(PC_bufc == 'd'){
                if(fgetc(read_fp) == 'e'){
                    if(fgetc(read_fp) == 'f'){
                        if(fgetc(read_fp) == 'i'){
                            if(fgetc(read_fp) == 'n'){
                                if(fgetc(read_fp) == 'e'){
                                    if((PC_bufc = fgetc(read_fp)) == ' ' || PC_bufc == '\t'){
                                        Precompiles->PC_type = DEFINE;
                                        int PC_index = 0;
                                        while((PC_bufc = fgetc(read_fp)) != ' ' && PC_bufc != '\t'){
                                            if(PC_bufc == '\n'){
                                                errorfound(SYNERR);
                                                break;
                                            }
                                            Precompiles->firstname[PC_index] = PC_bufc;
                                            PC_index++;
                                        }
                                        Precompiles->firstname[PC_index] = '\0';
                                        ungetc(PC_bufc, read_fp);
                                        while((PC_bufc = fgetc(read_fp)) == ' ' || PC_bufc == '\t' || PC_bufc == '\n'){
                                            if(PC_bufc == '\n'){
                                                ungetc('\n', read_fp);
                                                errorfound(SYNERR);
                                                break;
                                            }
                                        }
                                        PC_index = 0;
                                        ungetc(PC_bufc, read_fp);
                                        while((PC_bufc = fgetc(read_fp)) != ' ' && PC_bufc != '\t' && PC_bufc != '\n'){
                                            Precompiles->secondname[PC_index] = PC_bufc;
                                            PC_index++;
                                        }
                                        Precompiles->secondname[PC_index] = '\0';
                                        ungetc(PC_bufc, read_fp);
                                        if(PC_bufc != '\n'){
                                            while((PC_bufc = fgetc(read_fp)) != '\n'){
                                                if(PC_bufc != ' ' && PC_bufc != '\t'){
                                                    errorfound(SYNERR);
                                                }
                                            }
                                            ungetc('\n', read_fp);
                                        }
                                    }
                                    else{
                                        errorfound(SYNERR);
                                    }
                                }
                                else{
                                    errorfound(SYNERR);
                                }
                            }
                            else{
                                errorfound(SYNERR);
                            }
                        }
                        else{
                            errorfound(SYNERR);
                        }
                    }
                    else{
                        errorfound(SYNERR);
                    }
                }
                else{
                    errorfound(SYNERR);
                }
            }
            Precompiles->next = (PRECOMPILES*)malloc(sizeof(PRECOMPILES));
            Precompiles = Precompiles->next;
            Precompiles->next = NULL;
            return get_token();
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
        case '.':
            return DOT;
        case '~':
            return REVERSE;
        default:
            return EMPTY_TOKEN;
    }
    return EMPTY_TOKEN;
}
void to_string(enum token_kind tk, char* out_string){
    switch (tk) {
        case INT:
            strcpy(out_string, "int");
            break;
        case CHAR:
            strcpy(out_string, "char");
            break;
        case FLOAT:
            strcpy(out_string, "float");
            break;
        case INT_CONST:
            strcpy(out_string, "const int");
            break;
        case CHAR_CONST:
            strcpy(out_string, "cosnt char");
            break;
        case FLOAT_CONST:
            strcpy(out_string, "const float");
            break;
        case DIVIDE:
            strcpy(out_string, "/");
            break;
        case MULTI:
            strcpy(out_string, "*");
            break;
        case MOD:
            strcpy(out_string, "%");
            break;
        case PLUS:
            strcpy(out_string, "+");
            break;
        case MINUS:
            strcpy(out_string, "-");
            break;
        case GT:
            strcpy(out_string, ">");
            break;
        case GEQ:
            strcpy(out_string, ">=");
            break;
        case LT:
            strcpy(out_string, "<");
            break;
        case LEQ:    
            strcpy(out_string, "<=");
            break;
        case EQ:
            strcpy(out_string, "==");
            break;
        case NEQ:
            strcpy(out_string, "!=");
            break;
        case BAND:
            strcpy(out_string, "&");
            break;
        case XOR:
            strcpy(out_string, "^");
            break;
        case BOR:    
            strcpy(out_string, "|");
            break;
        case LAND:
            strcpy(out_string, "&&");
            break;
        case LOR:    
            strcpy(out_string, "||");
            break;
        case MODEQ:
            strcpy(out_string, "%=");
            break;
        case PLUSEQ:
            strcpy(out_string, "+=");
            break;
        case MINUSEQ:
            strcpy(out_string, "-=");
            break;
        case MULTIEQ:
            strcpy(out_string, "*=");
            break;
        case DIVIDEEQ:
            strcpy(out_string, "/=");
            break;
        case ASSIGN:
            strcpy(out_string, "=");
            break;
        case COMMA:
            strcpy(out_string, ",");
            break;
        case DBPLUS:
            strcpy(out_string, "++");
            break;
        case DBMINUS:
            strcpy(out_string, "--");
            break;
        case DOT:
            strcpy(out_string, ".");
            break;
        case CHOOSE:
            strcpy(out_string, "->");
            break;
        case NOT:
            strcpy(out_string, "!");
            break;
        case REVERSE:
            strcpy(out_string, "~");
            break;
        case RDBMINUS:
            strcpy(out_string, "IDENT--");
            break;
        case LDBMINUS:
            strcpy(out_string, "--IDENT");
            break;
        case RDBPLUS:
            strcpy(out_string, "IDENT++");
            break;
        case LDBPLUS:
            strcpy(out_string, "++IDENT");
            break;
        default:
            errorfound(INVALID);//invalid type
    }
}



int is_letter(char buf_ch){
    if((buf_ch >= 'a' && buf_ch <= 'z') || (buf_ch >= 'A' && buf_ch <= 'Z'))return 1;
    return 0;
}


int is_number(char buf_ch){
    if((buf_ch >= '0' && buf_ch <= '9'))return 1;
    return 0;
}

//单字节运算符
int check_single(enum token_kind tk){
    if (tk >= XOR) return 1;
    return 0;
}

//单目运算符
int check_oneside(enum token_kind tk){
    if (tk == NOT || tk == DBPLUS || tk == DBMINUS || tk == LDBPLUS || tk == RDBPLUS || tk == LDBMINUS || tk == RDBMINUS || tk == NOT || tk == REVERSE) return 1;
    return 0;
}

int check_special(enum token_kind tk){
    if(tk == LP || tk == RP || tk == LB || tk == RB || tk == SEMI || tk == COMMA)return 1;
    return 0;
}

int check_bothside(enum token_kind tk){
    if(check_operator(tk) && !check_oneside(tk) && !check_special(tk))return 1;
    return 0;
}


int check_const(enum token_kind tk){
    if(tk == INT_CONST)return INT;
    if(tk == FLOAT_CONST)return FLOAT;
    if(tk == CHAR_CONST)return CHAR;
    return EMPTY_TOKEN;
}
int check_bool(enum token_kind tk){
    switch (tk) {
        case GT:
            ;
        case GEQ:
            ;
        case LT:
            ;
        case LEQ:    
            ;
        case EQ:
            ;
        case NEQ:
            ;
        case LAND:
            ;
        case LOR: 
            return 1;
        default:
            return 0;
    }
}
int check_declare(enum token_kind tk){
    if (tk == INT || tk == FLOAT || tk == CHAR)return 1;
    return 0;
}

int check_operator(enum token_kind tk){
    return order_operator(tk);
}

int order_operator(enum token_kind tk){
    switch (tk) {
        case DOT:
            ;
        case CHOOSE:
            return 1;
        case RDBPLUS:
            ;
        case RDBMINUS:
            ;
        case LDBPLUS:
            ;
        case LDBMINUS:
            ;
        case DBPLUS:
            ;
        case DBMINUS:
            ;
        case REVERSE:
            ;
        case NOT:
            return 2;
        case DIVIDE:
            ;
        case MULTI:
            ;
        case MOD:
            return 3;
        case PLUS:
            ;
        case MINUS:
            return 4;
        case GT:
            ;
        case GEQ:
            ;
        case LT:
            ;
        case LEQ:    
            return 6;
        case EQ:
            ;
        case NEQ:
            return 7;
        case BAND:
            return 8;
        case XOR:
            return 9;
        case BOR:    
            return 10;
        case LAND:
            return 11;
        case LOR:    
            return 12;
        case MODEQ:
            ;
        case PLUSEQ:
            ;
        case MINUSEQ:
            ;
        case MULTIEQ:
            ;
        case DIVIDEEQ:
            ;
        case ASSIGN:
            return 14;
        case COMMA:
            return 15;
        case LP:
            ;
        case RP:
            return 16;
        default:
            return 0;//not an operator
    }
}

//1: >=, 0: <
int compare_operator(enum token_kind left, enum token_kind right){
    return (order_operator(left) <= order_operator(right));
}
    
void push_RNL(RNL** RNLp, char* ED_name, enum struct_type var_type, enum token_kind value_type){
//    if((*RNLp)->var_type != STARTOFSTA){
//        (*RNLp) = (*RNLp)->last;
//    }
    RNL* buf_RNL = (*RNLp);
    (*RNLp)->next = (RNL*)malloc(sizeof(RNL));
    (*RNLp) = (*RNLp)->next;
    (*RNLp)->last = buf_RNL;
    (*RNLp)->var_type = var_type;
    (*RNLp)->value_type = value_type;
    strcpy((*RNLp)->var_name, ED_name);
    (*RNLp)->FPL = (FPL*)malloc(sizeof(FPL));
    (*RNLp)->FPL->FP_kind = EMPTY_TOKEN;
    (*RNLp)->next = (RNL*)malloc(sizeof(RNL));
    (*RNLp)->next->value_type = EMPTY_TOKEN;
    (*RNLp)->next->var_type = ENDOFSTA;
    return;
}

//检查是否已声明
enum token_kind check_registed(RNL* RNL_cur, char* var_name, enum struct_type var_type, APL* APL){
    //printf("1");
//    if(RNL_cur != NULL){
//        RNL_cur = RNL_cur->next;
//    }
    while(RNL_cur->var_type != ENDOFSTA){
        if(RNL_cur->var_type == var_type){
            if(strcmp(RNL_cur->var_name, var_name) == 0){
                if(var_type == FUNCDEF){
                    FPL* FPL_buf = RNL_cur->FPL;
                    int same_status = 1;
                    while(APL->NMS->res_kind != EMPTY_TOKEN){
                        if(FPL_buf != NULL && FPL_buf->FP_kind == EMPTY_TOKEN){
                            same_status = 0;
                            break;
                        }
                        if(APL->NMS->res_kind != FPL_buf->FP_kind){
                            same_status = 0;
                            break;
                        }
                        APL = APL->next;
                        FPL_buf = FPL_buf->next;
                    }
                    if(same_status == 0){
                        continue;
                    }
                    if(FPL_buf->FP_kind != EMPTY_TOKEN)continue;
                    return RNL_cur->value_type;
                }
                else if(var_type == EXTVARDEF){
                    return RNL_cur->value_type;
                }
                else{
                    errorfound(INVALID);//invalid var_type
                }
            }
        }
        RNL_cur = RNL_cur->last;
    }
    return EMPTY_TOKEN;
}


//在当前块检查是否重复声明
int search_RNL(RNL* RNL_cur_root, char* var_name){
    while(RNL_cur_root->var_type != ENDOFSTA){
        if(strcmp(RNL_cur_root->var_name, var_name) == 0)return 1;
        RNL_cur_root = RNL_cur_root->next;
    }
    return 0;
}


void clear_RNL(RNL* clear_root){
    clear_root = clear_root->next;
    while(clear_root != NULL && clear_root->var_type != ENDOFSTA){
        //FPL在输出时已经free了
//        if(clear_root->FPL->FP_kind != EMPTY_TOKEN){
//            free(clear_root->FPL);
//            clear_root->FPL = NULL;
//        }
        RNL* buf_RNL = clear_root;
        clear_root = clear_root->next;
        free(buf_RNL);
        buf_RNL = NULL;
    }
}

void clear_EDL(EDL* clear_root){
    return;
}




