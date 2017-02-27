#ifndef __OVERLAY_H__
#define __OVERLAY_H__

#pragma define_section videoplayer_itcm ".videoplayer_itcm" ".videoplayer_itcm.bss" abs32 RWX

#define VIDEOPLAYER_OVERLAY_BEGIN	section videoplayer_itcm begin
#define VIDEOPLAYER_OVERLAY_END	section videoplayer_itcm end


#pragma define_section rendering_itcm ".rendering_itcm" ".rendering_itcm.bss" abs32 RWX

#define RENDERING_OVERLAY_BEGIN	section rendering_itcm begin
#define RENDERING_OVERLAY_END	section rendering_itcm end

#define LOAD_OVERLAY_ITCM(name)								\
do {														\
	int default_dma = FS_GetDefaultDMA();					\
	FS_SetDefaultDMA(FS_DMA_NOT_USE);						\
	FS_EXTERN_OVERLAY(name);								\
	FS_LoadOverlay(MI_PROCESSOR_ARM9, FS_OVERLAY_ID(name));	\
	FS_SetDefaultDMA(default_dma);							\
} while (0);

#endif