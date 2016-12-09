#ifndef __MENU_H__
#define __MENU_H__

#define MENU_PRIVATE_HEAP_GROUP_ID	0x5A

class Menu
{
public:
	virtual void Run(int arg) = 0;
};

extern Menu* gRunningMenu;
extern int gNextMenuArg;

typedef Menu* (*OnMenuCreate)();

extern OnMenuCreate gNextMenuCreateFunc;

#endif