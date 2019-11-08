#pragma once

#pragma pack(1)

#include "AttemperEngineSink.h"
#include <vector>

using std::vector;

#define RANK_COUNT  12  //���а��������

#define RANK_VEC	vector<tagRankInfo*>

//���а������
class RankManager
{
public:
	RankManager();
	~RankManager(void);

	static RankManager* Instance();
	void Init();

protected:
	static RankManager* s_instance;

	RANK_VEC vecTodayWast;		//�������ķ���
	RANK_VEC vecTodayWin;		//����ʤ��
	RANK_VEC vecYseterdatWast; //�������ķ���
	RANK_VEC vecYesterdayWin;	//����ʤ��
	RANK_VEC vecWeekWast;		//�������ķ���
	RANK_VEC vecWeekWin;		//����ʤ��
	RANK_VEC vecMonthWast;		//�������ķ���
	RANK_VEC vecMonthWin;		//����ʤ��

	RANK_VEC vecRecharge;		//��ֵ��

	RANK_VEC vecPlayGoldJuCount;	//��ɽ�ҳ�����
	RANK_VEC vecPlayJjcCount;		//��ɾ�������Ϸ����
	RANK_VEC vecPlayFkCount;		//��ɷ�����Ϸ����

protected:

	bool		m_bRecived[RANK_COUNT];						//�����Ƿ���ȡ
	RANK_VEC*	m_vecArry[RANK_COUNT];						//�б�ָ������

	HANDLE		m_ArrayMutex;			//m_vecArryʹ�û�����

public:
	//���ӽ�������
	void AddTodayWast(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//���ӽ���ʤ��
	void AddTodayWin(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//������������
	void AddYseterdatWast(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//��������ʤ��
	void AddYseterdatWin(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//���ӱ�������
	void AddWeekWast(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//���ӱ���ʤ��
	void AddWeekWin(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//���ӱ�������
	void AddMonthWast(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//���ӱ���ʤ��
	void AddMonthWin(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);

	//���ӳ�ֵ���
	void AddRecharge(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//���������ҳ�����
	void AddPlayGoldJuCount(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//�������澺��������
	void AddPlayJjcCount(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);
	//�������淿��������
	void AddPlayFkCount(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl);

	//ͨ����ӷ���
	void AddRankCount(DWORD dwUserID, DWORD dwCount,TCHAR* szNickName, TCHAR* szHeadUrl, WORD index);

	
public:
	//�����ݿ��ȡ���а��б�
	void InitRankList(IDataBase *DataBase);
	//����
	void SortRankList();
	//�����б�����
	void SortRankListByIndex(int index);
	//��ȡ�ƶ�������
	WORD GetRankCountByIndex(int index);
	//��ȡ�ƶ����а�Ķ�Ӧ������Ϣ
	tagRankInfo* GetRankItemByIndex(int index, int byRankNum);
	//��ȡ���׽���
	int GetRankReward(int index, DWORD dwUserID, TCHAR szDescribe[], DWORD dwContextID);

protected:
	void Lock() //ռ�û�����
	{
		// �ȴ��������֪ͨ
		WaitForSingleObject(m_ArrayMutex, INFINITE);
	};

	void UnLock() //�ͷŻ�����
	{
		ReleaseMutex(m_ArrayMutex);
	};

};

