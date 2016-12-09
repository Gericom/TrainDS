#include <nitro.h>
#include "core.h"
#include "Menu.h"

Menu* gRunningMenu = NULL;
int gNextMenuArg = 0;

OnMenuCreate gNextMenuCreateFunc = NULL;