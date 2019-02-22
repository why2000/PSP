//
//  output.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include "output.h"
extern FILE* read_fp;//"main.c"
extern RNL* rootRNL;//"main.c"
extern RNL* leaveRNL;//"main.c"

//output to console
int EDLoutput(EDL* curEDL, FILE* write_fp){
    printf("output test!\n");
    
    if(write_fp != NULL){
        EDLFILEoutput(curEDL, write_fp);
    }
    return 0;
}

//output to file
int EDLFILEoutput(EDL* curEDL, FILE* write_fp){
    return 0;
}
