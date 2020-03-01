# ifndef _SUB_RECORD_H_
# define _SUB_RECORD_H_

# include "stdafx.h"
# include "PlayerManager.h"
# include <vector>

class CSubRecord
{
	//录像缓存
private:
	BYTE		m_cbRecordData[7164];		//数据缓存
	BYTE		m_cbRecordAction[1024];		//数据标识
	BYTE		m_cbMerge[LEN_MAX_RECORD_SIZE];			//合并

	//录像参数
private:
	WORD		m_wActionCount;				//act标识位
	DWORD		m_wDataSize;				//Data存储位置

public:
	CSubRecord(void) {}
	~CSubRecord(void) {}

	//功能函数
public:
	//初始化
	void Init();
	//插入数据
	void AddRecord(WORD wChairID, WORD msgID, VOID *msgData, DWORD dwDataSize);
	//插入玩家信息
	void AddPlayer(std::vector<CPlayer*> list);
	//获取数据
	VOID *GetData(WORD wPlayerCount, DWORD &dwDataSize);
};

# endif