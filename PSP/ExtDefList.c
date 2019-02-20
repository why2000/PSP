//
//  ExtDefList.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include "ExtDefList.h"
EDL* ExtDefList(FILE* read_fp){
    enum token_kind cur_tk;
    EDL* curEDL = (EDL*)malloc(sizeof(EDL));
    cur_tk = get_token(read_fp);
    
    return curEDL;
}
