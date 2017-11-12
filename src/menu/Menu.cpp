#include "common.h"

#include "Menu.h"

Menu* gRunningMenu = NULL;
int gNextMenuArg = 0;

OnMenuCreate gNextMenuCreateFunc = NULL;