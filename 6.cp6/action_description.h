#pragma once

#include "river_def.h"

typedef struct tagActionDescription
{
    ACTION_NAME act;
    const char *description; // 相应动作的人类语言描述
} ACTION_DESCRIPTION;

const char *GetActionDescription(ACTION_NAME act);
