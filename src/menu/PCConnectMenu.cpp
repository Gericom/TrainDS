#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/g3d.h>
#include <nnsys/gfd.h>
#include <dwc.h>
#include "core.h"
#include "util.h"
#include "Menu.h"
#include "TitleMenu.h"
#include "PCConnectMenu.h"

void PCConnectMenu::Initialize(int arg)
{
	Core_StopTPSampling();
	void* work = NNS_FndAllocFromExpHeapEx(gHeapHandle, DWC_UTILITY_WORK_SIZE, 32);
	DWC_StartUtility(work, DWC_LANGUAGE_ENGLISH, DWC_UTILITY_TOP_MENU_FOR_EUR);
	NNS_FndFreeToExpHeap(gHeapHandle, work);
	Core_SetupSndArc();
	Core_StartTPSampling();
	GX_DispOn();
	GXS_DispOn();
	TitleMenu::GotoMenu();
}

void PCConnectMenu::Render()
{

}

void PCConnectMenu::VBlank()
{

}

void PCConnectMenu::Finalize()
{

}