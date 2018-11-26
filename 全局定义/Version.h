#ifndef VERSION_FRAME_HEAD_FILE
#define VERSION_FRAME_HEAD_FILE

#pragma pack(1)

/************************************************************************
** CONFIG  配置文件 
** 备注1: 只能修改对应的版本信息, 非自己的版本即使修改了也没用
          内核只能修改代码版本和内核版本
		  framework只能修改平台版本
		  子游戏只能修改子游戏版本
************************************************************************/

/* 游戏版本信息 */
//框架版本 1-254; 255为IUnknownEx
const BYTE PLATFORM_VERSION = 
/* PLATFORM_VERSION: */      4;

//子游戏版本 1-254; 255为IUnknownEx
const BYTE SUBGAME_VERSION =
/* SUBGAME_VERSION: */    0;

#pragma pack()

#endif