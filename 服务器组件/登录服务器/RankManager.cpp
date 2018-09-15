#include "stdafx.h"
#include "RankManager.h"
#include "DataBasePacket.h"

RankManager::RankManager(IDataBaseEngine* pIDataBaseEngine)
{
	m_vecArry[0] = &vecTodayWast;
	m_vecArry[1] = &vecYseterdatWast;
	m_vecArry[2] = &vecWeekWast;
	m_vecArry[3] = &vecMonthWast;
	m_vecArry[4] = &vecTodayWin;
	m_vecArry[5] = &vecYesterdayWin;
	m_vecArry[6] = &vecWeekWin;
	m_vecArry[7] = &vecMonthWin;
	m_vecArry[8] = &vecRecharge;
	m_vecArry[9] = &vecPlayGoldJuCount;
	m_vecArry[10] = &vecPlayJjcCount;
	m_vecArry[11] = &vecPlayFkCount;

	m_ArrayMutex = CreateMutex(NULL,FALSE,NULL);

	RANK_VEC* vecPtr = NULL;
	for(int i=0;i < RANK_COUNT;i++)
	{
		vecPtr = m_vecArry[i];
		if(NULL != vecPtr)
		{
			vecPtr->clear();
		}
		m_bRecived[i] = false;
	}
	//������ݿ�����
	m_pIDataBaseEngine = pIDataBaseEngine;

	//��ʼ��
	Init();
}


RankManager::~RankManager(void)
{
	//�ͷ�new�ڵ�
	Lock();
	RANK_VEC* vecPtr = NULL;
	for(int i=0;i < RANK_COUNT;i++)
	{
		vecPtr = m_vecArry[i];
		if(NULL != vecPtr)
		{
			for(DWORD j=0;j < vecPtr->size();j++)
			{
				if(vecPtr->at(j) != NULL) 
				{
					delete vecPtr->at(j);
					vecPtr->at(j) = NULL;
				}
			}
			vecPtr->clear();
		}
		m_bRecived[i] = false;
	}
	UnLock();
}


void RankManager::Init()
{
	if(NULL != m_pIDataBaseEngine)
	{
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_READ_RANK_LIST,0,this,sizeof(RankManager));
	}
}


//���ӽ�������
void RankManager::AddTodayWast(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl)
{
	AddRankCount(dwUserID,dwCount,szNickName,szHeadUrl,0);
}

//���ӽ���ʤ��
void RankManager::AddTodayWin(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl)
{
	AddRankCount(dwUserID,dwCount,szNickName,szHeadUrl,1);
}

//������������
void RankManager::AddYseterdatWast(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl)
{
	AddRankCount(dwUserID,dwCount,szNickName,szHeadUrl,2);
}

//��������ʤ��
void RankManager::AddYseterdatWin(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl)
{
	AddRankCount(dwUserID,dwCount,szNickName,szHeadUrl,3);
}

//���ӱ�������
void RankManager::AddWeekWast(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl)
{
	AddRankCount(dwUserID,dwCount,szNickName,szHeadUrl,4);
}

//���ӱ���ʤ��
void RankManager::AddWeekWin(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl)
{
	AddRankCount(dwUserID,dwCount,szNickName,szHeadUrl,5);
}

//���ӱ�������
void RankManager::AddMonthWast(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl)
{
	AddRankCount(dwUserID,dwCount,szNickName,szHeadUrl,6);
}

//���ӱ���ʤ��
void RankManager::AddMonthWin(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl)
{
	AddRankCount(dwUserID,dwCount,szNickName,szHeadUrl,7);
}

//���ӳ�ֵ���
void RankManager::AddRecharge(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl)
{
	AddRankCount(dwUserID,dwCount,szNickName,szHeadUrl,8);
}

//���������ҳ�����
void RankManager::AddPlayGoldJuCount(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl)
{
	AddRankCount(dwUserID,dwCount,szNickName,szHeadUrl,9);
}

//�������澺��������
void RankManager::AddPlayJjcCount(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl)
{
	AddRankCount(dwUserID,dwCount,szNickName,szHeadUrl,10);
}

//�������淿��������
void RankManager::AddPlayFkCount(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl)
{
	AddRankCount(dwUserID,dwCount,szNickName,szHeadUrl,11);
}

//ͨ����ӷ���
void RankManager::AddRankCount(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl, WORD index)
{
	if(index < 0 || index >= RANK_COUNT||szNickName == NULL || szHeadUrl == NULL)
		return;

	Lock();

	RANK_VEC* vecPtr = m_vecArry[index];
	if(NULL == vecPtr)
	{
		UnLock();
		return;
	}
	bool bFind = false;
	int i = 0;
	for(DWORD i=0;i < vecPtr->size();i++)
	{
		if(vecPtr->at(i)->dwUserID == dwUserID)
		{
			bFind = true;
			vecPtr->at(i)->dwCount += dwCount;
			break;
		}
	}

	//�б���δ�ҵ�
	if(!bFind)
	{
		tagRankInfo* item = new tagRankInfo;
		item->byRankNum = vecPtr->size() + 1;
		item->byReceived = 0;
		item->dwUserID = dwUserID;
		item->dwCount = dwCount;
		lstrcpyn(item->szNickName,szNickName,CountArray(item->szNickName));
		lstrcpyn(item->szHeadUrl,szHeadUrl,CountArray(item->szHeadUrl));

		vecPtr->push_back(item);
		i = vecPtr->size();
	}
	UnLock();
	//����
	SortRankListByIndex(i);

	DBR_GP_UserRankItem dbrRankItem;
	ZeroMemory(&dbrRankItem,sizeof(DBR_GP_UserRankItem));

	dbrRankItem.dwUserID = vecPtr->at(i)->dwUserID;
	dbrRankItem.byRankNum = vecPtr->at(i)->byRankNum;
	dbrRankItem.byReceived = vecPtr->at(i)->byReceived;
	dbrRankItem.dwCount = vecPtr->at(i)->dwCount;
	dbrRankItem.byIndex =  static_cast<BYTE>(index);

	

	//д�����ݿ�
	if(NULL != m_pIDataBaseEngine)
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_RANK_VALUE,0,&dbrRankItem,sizeof(DBR_GP_UserRankItem));
}


void RankManager::InitRankList(CDataBaseAide &DataBaseAide, CDataBaseHelper &DataBaseHelper)
{
	DWORD dwCount[RANK_COUNT];
	for(int i = 0;i < DataBaseHelper->GetRecordCount();i++,DataBaseHelper->MoveToNext())
	{
		memset(dwCount,0,sizeof(dwCount));

		DWORD UserID = DataBaseAide.GetValue_DWORD(TEXT("UserID"));
		dwCount[0] = DataBaseAide.GetValue_DWORD(TEXT("ToDayWast"));
		dwCount[1] = DataBaseAide.GetValue_DWORD(TEXT("YesterdayWast"));
		dwCount[2] = DataBaseAide.GetValue_DWORD(TEXT("WeekWast"));
		dwCount[3] = DataBaseAide.GetValue_DWORD(TEXT("MonthWast"));
		dwCount[4] = DataBaseAide.GetValue_DWORD(TEXT("ToDayWin"));
		dwCount[5] = DataBaseAide.GetValue_DWORD(TEXT("YesterdayWin"));
		dwCount[6] = DataBaseAide.GetValue_DWORD(TEXT("WeekWin"));
		dwCount[7] = DataBaseAide.GetValue_DWORD(TEXT("MonthWin"));
		dwCount[8] = DataBaseAide.GetValue_DWORD(TEXT("Recharge"));
		dwCount[9] = DataBaseAide.GetValue_DWORD(TEXT("PlayGoldJuCount"));
		dwCount[10] = DataBaseAide.GetValue_DWORD(TEXT("PlayJjcCount"));
		dwCount[11] = DataBaseAide.GetValue_DWORD(TEXT("PlayFkCount"));

		TCHAR szNickName[LEN_NICKNAME];
		TCHAR szHeadUrl[256];
		DataBaseAide.GetValue_String(TEXT("NickName"),szNickName,CountArray(szNickName));
		DataBaseAide.GetValue_String(TEXT("HeadUrl"),szHeadUrl,CountArray(szHeadUrl));

		for(int j=0;j < RANK_COUNT;j++)
		{
			if(UserID == 0)
			{
				m_bRecived[i] = (dwCount[i] > 0);
			}else
			{
				RANK_VEC* vecPtr = m_vecArry[j];
				tagRankInfo *item = new tagRankInfo;
				item->dwUserID = UserID;
				item->byRankNum = 0;
				item->dwCount = dwCount[j];
				item->byReceived = 0;
				item->byIndex = j;
				lstrcpyn(item->szNickName,szNickName,CountArray(szNickName));
				lstrcpyn(item->szHeadUrl,szHeadUrl,CountArray(szHeadUrl));

				vecPtr->push_back(item);
			}
		}
	}

	//�����Ƿ���ȡ
	for(int i=0;i < RANK_COUNT;i++)
	{
		RANK_VEC* vecPtr = m_vecArry[i];
		for(DWORD j=0;j < vecPtr->size();j++)
		{
			vecPtr->at(j)->byReceived = m_bRecived[i]?1:0;	
		}
	}

	//����
	SortRankList();
}


void RankManager::SortRankList()
{
	for(int i=0;i < RANK_COUNT;i++)
	{
		SortRankListByIndex(i);
	}
}

//�����б�����
void RankManager::SortRankListByIndex(int index)
{
	Lock();
	//ð�����򣬴Ӵ�С
	RANK_VEC* vecPtr = m_vecArry[index];
	for(DWORD j=0;NULL != vecPtr && j < vecPtr->size();j++)
	{
		for(DWORD k=0;k+1 < vecPtr->size() - j;k++)
		{
			if(vecPtr->at(k)->dwCount < vecPtr->at(k+1)->dwCount)
			{
				//����λ��
				tagRankInfo* temp = vecPtr->at(k);
				vecPtr->at(k) = vecPtr->at(k+1);
				vecPtr->at(k+1) = temp;
			}
		}
	}

	//�������,��������
	int nNum = 1;
	for(DWORD n=0;n < vecPtr->size();n++)
	{
		vecPtr->at(n)->byRankNum = nNum;
		/*if(n < vecPtr->size() - 1)
		{
			if(vecPtr->at(n)->dwCount != vecPtr->at(n+1)->dwCount)
			{*/
				nNum++;
		/*	}
		}*/
	}
	UnLock();
}

WORD RankManager::GetRankCountByIndex(int index)
{
	RANK_VEC* vecPtr = m_vecArry[index];
	if(NULL == vecPtr)
		return 0;
	return vecPtr->size();
}

tagRankInfo* RankManager::GetRankItemByIndex(int index, int rankNum)
{
	RANK_VEC* vecPtr = m_vecArry[index];
	if(NULL == vecPtr)
		return NULL;

	//for(int i =0;i < vecPtr->size() ;i++)
	{
		//if(vecPtr->at(i)->byRankNum == rankNum)
		{
			return (vecPtr->at(rankNum));
		}
	}

	return NULL;
}

//��ȡ���׽���
int RankManager::GetRankReward(int index, DWORD dwUserID, TCHAR szDescribe[], DWORD dwSocketID)
{
	//�ж��Ƿ�������ȡ����
	if(index > 7 || index < 0)
	{
		lstrcpyn(szDescribe,TEXT("�����쳣!"),127);
		return 2;
	}

	Lock();

	RANK_VEC* vecPtr = m_vecArry[index];
	if(NULL == vecPtr)
	{
		lstrcpyn(szDescribe,TEXT("�����쳣!"),127);
		UnLock();
		return 2;
	}

	for(DWORD i=0;i < vecPtr->size();i++)
	{
		if(vecPtr->at(i)->dwUserID == dwUserID && vecPtr->at(i)->byRankNum == 1 && !m_bRecived[i])
		{
			//��������
			STR_DBR_CL_SERCIVR_GET_RANK_REWARD RecRankReward;
			ZeroMemory(&RecRankReward,sizeof(RecRankReward));

			RecRankReward.byIndex = index;
			RecRankReward.dwUserID = dwUserID;

			//Ͷ������
			if(true == m_pIDataBaseEngine->PostDataBaseRequest(DBR_CL_SERVICE_GET_RANK_REWARD,dwSocketID,&RecRankReward,sizeof(STR_DBR_CL_SERCIVR_GET_RANK_REWARD)))
			{
				m_bRecived[i] = true;
				UnLock();
				return 0;
			}
		}
	}
	UnLock();

	if(NULL == vecPtr)
	{
		lstrcpyn(szDescribe,TEXT("��Ǹ,����δ������ȡ����!"),127);
		return 3;
	}

	lstrcpyn(szDescribe,TEXT("�����쳣!"),127);
	return 2;
}