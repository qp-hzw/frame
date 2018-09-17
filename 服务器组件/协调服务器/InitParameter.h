#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParameter
{
	//系统消息(跑马灯)yang TODONOW added by WangChengQing  跑马灯消息放入数据库中配置
public:
	WORD							m_wMarqueeMsgPlayCount;				//消息播放次数
	WORD							m_wMarqueeMsgApartTime;				//消息播放间隔时间
	WORD							m_wSYSPlayTime ;					//系统消息播放时间
	WORD							m_wGamePlayTime;					//游戏消息播放时间


	//函数定义
public:
	//构造函数
	CInitParameter();
	//析构函数
	virtual ~CInitParameter();

	//功能函数
public:
	//初始化
	VOID InitParameter();
	//加载配置
	VOID LoadInitParameter();
	//加载配置(新增)yang
	VOID LoadMsgParameter();
};

//////////////////////////////////////////////////////////////////////////////////

#endif
