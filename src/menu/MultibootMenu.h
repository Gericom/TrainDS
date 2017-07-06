#ifndef __MULTIBOOTMENU_H__
#define __MULTIBOOTMENU_H__

#include "SimpleMenu.h"

class MultibootMenu : public SimpleMenu
{
public:
	MultibootMenu() : SimpleMenu(17, 17)
	{ }

	void Initialize(int arg);

	void Render();
	void VBlank();
	void Finalize();

	static void GotoMenu()
	{
		gNextMenuArg = 0;// TITLEMENU_ARG_DONT_PLAY_INTRO;
		gNextMenuCreateFunc = CreateMenu;
	}

private:
	static Menu* CreateMenu()
	{
		return new MultibootMenu();
	}
};

#endif