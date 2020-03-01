# ifndef _SUB_RECORD_H_
# define _SUB_RECORD_H_

# include "stdafx.h"
# include "PlayerManager.h"
# include <vector>

class CSubRecord
{
	//¼�񻺴�
private:
	BYTE		m_cbRecordData[7164];		//���ݻ���
	BYTE		m_cbRecordAction[1024];		//���ݱ�ʶ
	BYTE		m_cbMerge[LEN_MAX_RECORD_SIZE];			//�ϲ�

	//¼�����
private:
	WORD		m_wActionCount;				//act��ʶλ
	DWORD		m_wDataSize;				//Data�洢λ��

public:
	CSubRecord(void) {}
	~CSubRecord(void) {}

	//���ܺ���
public:
	//��ʼ��
	void Init();
	//��������
	void AddRecord(WORD wChairID, WORD msgID, VOID *msgData, DWORD dwDataSize);
	//���������Ϣ
	void AddPlayer(std::vector<CPlayer*> list);
	//��ȡ����
	VOID *GetData(WORD wPlayerCount, DWORD &dwDataSize);
};

# endif