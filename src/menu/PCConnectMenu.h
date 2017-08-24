#pragma once

#include "SimpleMenu.h"

class PCConnectMenu : public SimpleMenu
{
public:
	PCConnectMenu() : SimpleMenu(17, 17)
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
		return new PCConnectMenu();
	}
};