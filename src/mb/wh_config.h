/*
  Project:  NitroSDK - wireless_shared - demos - wh - default
  File:     wh_config.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wh_config.h,v $
  Revision 1.6  2007/10/03 01:05:11  okubata_ryoma
  Copyright fix

  Revision 1.5  2007/10/02 09:05:32  yosizaki
  Small fix about WM_SIZE_DS_PARENT_HEADER.

  Revision 1.4  2006/04/11 00:01:31  okubata_ryoma
  Copyright fix

  Revision 1.3  2006/04/10 13:18:57  yosizaki
  Added support for WH_MP_FREQUENCY.

  Revision 1.2  2005/12/21 02:36:44  adachi_hiroaki
  Changed identifier in "define" statement

  Revision 1.1  2005/12/21 02:20:17  adachi_hiroaki
  Changed the location of wh_config.h.

  Revision 1.2  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.1  2004/12/20 07:17:48  takano_makoto
  Wireless communications parameters for each application are separated as wh_config.h

  $NoKeywords$
 */

#ifndef _DEFAULT_WH_CONFIG_H__
#define _DEFAULT_WH_CONFIG_H__


 // DMA number used for wireless communications
#define WH_DMA_NO                 2

#define WH_PLAYER_MAX             4

 // Maximum number of children (this number does not include the parent)
#define WH_CHILD_MAX              (WH_PLAYER_MAX - 1)

 // Maximum size of data that can be shared
#define WH_DS_DATA_SIZE           32

#define WH_PARENT_MAX_SIZE        512
#define WH_CHILD_MAX_SIZE       (WH_DS_DATA_SIZE)

 // Port to use for normal MP communications
#define WH_DATA_PORT              14

 // Priority to use for normal MP communications
#define WH_DATA_PRIO              WM_PRIORITY_URGENT

 // to continuous transfer mode to combine WFS and data sharing */
#define WH_MP_FREQUENCY           0

 // Port to use for data sharing
#define WH_DS_PORT                13

 // Port to use for WFS
#define PORT_WFS                  4



#endif // _DEFAULT_WH_CONFIG_H__
