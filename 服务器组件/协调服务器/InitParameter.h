#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//���ò���
class CInitParameter
{
	//ϵͳ��Ϣ(�����)yang TODONOW added by WangChengQing  �������Ϣ�������ݿ�������
public:
	WORD							m_wMarqueeMsgPlayCount;				//��Ϣ���Ŵ���
	WORD							m_wMarqueeMsgApartTime;				//��Ϣ���ż��ʱ��
	WORD							m_wSYSPlayTime ;					//ϵͳ��Ϣ����ʱ��
	WORD							m_wGamePlayTime;					//��Ϸ��Ϣ����ʱ��


	//��������
public:
	//���캯��
	CInitParameter();
	//��������
	virtual ~CInitParameter();

	//���ܺ���
public:
	//��ʼ��
	VOID InitParameter();
	//��������
	VOID LoadInitParameter();
	//��������(����)yang
	VOID LoadMsgParameter();
};

//////////////////////////////////////////////////////////////////////////////////

#endif
