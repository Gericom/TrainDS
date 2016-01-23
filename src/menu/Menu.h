#ifndef __MENU_H__
#define __MENU_H__

class Menu
{
public:
	virtual void Initialize(int arg) = 0;
	virtual void Render() = 0;
	virtual void VBlank() = 0;
	virtual void Finalize() = 0;
};

#endif