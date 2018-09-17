#ifndef LOGIN_TRACE_HELPER_HEAD_FILE
#define LOGIN_TRACE_HELPER_HEAD_FILE

#pragma once
class LogicTraceHelper
{
private:
	LPCTSTR m_pszText;
	bool m_bDone;

public:
	LogicTraceHelper(LPCTSTR text);
	~LogicTraceHelper(void);

	inline void Mark(bool done)
	{
		m_bDone = done;
	}
};


#endif