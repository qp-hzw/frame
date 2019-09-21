#include "stdafx.h"
#include "LogicTraceHelper.h"


LogicTraceHelper::LogicTraceHelper(LPCTSTR text)
	: m_pszText(text)
	, m_bDone(true)
{
	CLog::TraceStringEx(
		log_debug, 
		TEXT("Enter %s"), 
		text);
}

LogicTraceHelper::~LogicTraceHelper(void)
{
	CLog::TraceStringEx(
		log_debug, 
		TEXT("Leave %s %s"), 
		m_pszText, 
		m_bDone?TEXT("finished"):TEXT("failed"));
}
