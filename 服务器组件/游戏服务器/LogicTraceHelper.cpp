#include "stdafx.h"
#include "LogicTraceHelper.h"


LogicTraceHelper::LogicTraceHelper(LPCTSTR text)
	: m_pszText(text)
	, m_bDone(true)
{
	CTraceService::TraceStringEx(
		TraceLevel_Debug, 
		TEXT("Enter %s"), 
		text);
}

LogicTraceHelper::~LogicTraceHelper(void)
{
	CTraceService::TraceStringEx(
		TraceLevel_Debug, 
		TEXT("Leave %s %s"), 
		m_pszText, 
		m_bDone?TEXT("finished"):TEXT("failed"));
}
