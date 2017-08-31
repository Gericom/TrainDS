#pragma once

#define LYT_RES_SIGNATURE_LYT1	MKTAG('l', 'y', 't', '1')
#define LYT_RES_SIGNATURE_PAN1	MKTAG('p', 'a', 'n', '1')
#define LYT_RES_SIGNATURE_PIC1	MKTAG('p', 'i', 'c', '1')
#define LYT_RES_SIGNATURE_TXT1	MKTAG('t', 'x', 't', '1')

struct pane_flags_t
{
	u16 visible : 1;
	u16 touchable : 1;
	u16 hOrigin : 2;
	u16 vOrigin : 2;
	u16 hParentCenter : 2;
	u16 vParentCenter : 2;
	u16 : 6;
};

#define PANE_H_ORIGIN_LEFT		0
#define PANE_H_ORIGIN_CENTER	1
#define PANE_H_ORIGIN_RIGHT		2

#define PANE_V_ORIGIN_TOP		0
#define PANE_V_ORIGIN_CENTER	1
#define PANE_V_ORIGIN_BOTTOM	2

#define PANE_H_PARENT_CENTER_LEFT	0
#define PANE_H_PARENT_CENTER_CENTER	1
#define PANE_H_PARENT_CENTER_RIGHT	2

#define PANE_V_PARENT_CENTER_TOP	0
#define PANE_V_PARENT_CENTER_CENTER	1
#define PANE_V_PARENT_CENTER_BOTTOM	2

struct text_pane_flags_t
{
	u8 textHAlignment : 2;
	u8 textVAlignment : 2;
	u8 : 4;
};

#define TEXT_PANE_TEXT_HALIGNMENT_LEFT		0
#define TEXT_PANE_TEXT_HALIGNMENT_CENTER	1
#define TEXT_PANE_TEXT_HALIGNMENT_RIGHT		2

#define TEXT_PANE_TEXT_VALIGNMENT_TOP		0
#define TEXT_PANE_TEXT_VALIGNMENT_CENTER	1
#define TEXT_PANE_TEXT_VALIGNMENT_BOTTOM	2

struct lyt_res_header_t
{
	u32 signature;
	u32 filesize;
	u16 width;
	u16 height;
	u32 rootpaneoffset;
	u32 stringtableoffset;
};

struct lyt_res_fnt1_t
{
	u32 signature;
	u32 blocksize;
	u32 nrfonts;
	u32 fontnameoffsets[0];
};

struct lyt_res_pan1_t
{
	u32 signature;
	u32 blocksize;
	u32 nameoffset;
	u16 nrchildren;
	pane_flags_t flags;
	s16 xtranslation;
	s16 ytranslation;
	fx32 rotation;
	fx16 scalex;
	fx16 scaley;
	u16 width;
	u16 height;
};

struct lyt_res_pic1_t : lyt_res_pan1_t
{
	u16 cell;
	u16 padding;
};

struct lyt_res_txt1_t : lyt_res_pan1_t
{
	u16 fontid;
	u8 paletteidx;
	text_pane_flags_t textflags;
	u8 characterspacing;
	u8 linespacing;
	u16 padding;
	u32 textoffset;
};

struct lyt_res_t
{
	lyt_res_header_t header;
	lyt_res_fnt1_t fonts;
	//lyt_res_pan1_t rootpane;
};