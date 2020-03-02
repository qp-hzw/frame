#include "SubRecord.h"
#include "TableManager.h"

//��ʼ��
void CSubRecord::Init()
{
	ZeroMemory(&m_cbRecordData, sizeof(BYTE)*7164);
	ZeroMemory(&m_cbRecordAction, sizeof(BYTE)*1024);
	ZeroMemory(&m_cbMerge, sizeof(BYTE)*LEN_MAX_RECORD_SIZE);
	m_wActionCount = 0;
	m_wDataSize = 0;
}

//��������
void CSubRecord::AddRecord(WORD wChairID, WORD msgID, VOID *msgData, DWORD dwDataSize)
{
	//����Act msgid
	memcpy(m_cbRecordAction+m_wActionCount*sizeof(WORD), &msgID, sizeof(WORD));
	m_wActionCount++;

	//����Data   chairid + msgdata
	memcpy(m_cbRecordData+m_wDataSize, &wChairID, sizeof(WORD));
	memcpy(m_cbRecordData+m_wDataSize+sizeof(WORD), msgData, dwDataSize);
	m_wDataSize += dwDataSize + sizeof(WORD);
}

//���������Ϣ
void CSubRecord::AddPlayer(std::vector<CPlayer*> list)
{
	for (int i = 0; i < list.size(); i++)
	{
		CPlayer *player = list[i];
		if (player)
		{
			CTableFrame *table = CTableManager::FindTableByTableID(player->GetTableID());
			if (table == NULL)	return;

			RecodePlayerInfo info;
			ZeroMemory(&info, sizeof(RecodePlayerInfo));

			//��ֵ
			info.wChairID = i;
			info.dwUserID = player->GetUserID();
			memcpy(info.szName, player->GetNickName(), sizeof(TCHAR)*LEN_NICKNAME);
			memcpy(info.szHeadUrl, player->GetUserInfo()->szHeadUrl, sizeof(TCHAR)*LEN_HEAD_URL);
			info.llScore = table->GetTotalScore()[i];

			CLog::Log(log_debug, "info.llScore: %d", info.llScore);

			//�����Ϣ���뵽Data��ͷ
			memcpy(m_cbRecordData+m_wDataSize, &info, sizeof(RecodePlayerInfo));
			m_wDataSize += sizeof(RecodePlayerInfo);
		}
	}
}

//��ȡ����
VOID * CSubRecord::GetData(WORD wPlayerCount, DWORD &dwDataSize)
{
	//��ʼ��
	dwDataSize = 0;
	m_wActionCount++;

	//�ϲ� 1����������
	memcpy(m_cbMerge, &m_wActionCount, sizeof(WORD));
	dwDataSize += sizeof(WORD);

	//2����Ҹ���
	memcpy(m_cbMerge+dwDataSize, &wPlayerCount, sizeof(WORD));
	dwDataSize += sizeof(WORD);

	//3����Ϣ��  msgid
	memcpy(m_cbMerge+dwDataSize, m_cbRecordAction, (m_wActionCount-1)*sizeof(WORD));
	dwDataSize += (m_wActionCount-1)*sizeof(WORD);

	//4���ṹ������  playerinfo + (chairid + msgdata)
	memcpy(m_cbMerge+dwDataSize, m_cbRecordData, m_wDataSize);
	dwDataSize += m_wDataSize;

	CLog::Log(log_debug, "m_wActionCount: %d", m_wActionCount);
	CLog::Log(log_debug, "dwDataSize: %d", dwDataSize);

	return m_cbMerge;
}