//
//  output.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#include "output.h"
//output to console
int EDLoutput(EDL* curEDL, FILE* fp){
    
    
    if(fp != NULL){
        EDLFILEoutput(curEDL, fp);
    }
}

//output to file
int EDLFILEoutput(EDL* curEDL, FILE* fp){
    
}
