//
//  ExtVarDef.h
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/21.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

#pragma once

#include "main.h"

EVD* ExtVarDef(enum token_kind, char*);
EVNL* ExtVarNameList(EVNL*, enum token_kind, RNL**);
