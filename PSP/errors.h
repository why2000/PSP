//
//  errors.h
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#pragma once

#include "main.h"
enum errortype { SYNERR = 0, TMRIGHT, TMLEFT, UNREGISTED, REDUNDANT, INVALID, DIFFOP, ASSIGNERR, ELSEONLY, RETERR, UNSUPPORTEDYET, ERRONESIDE};
void* errorfound(enum errortype index);
