#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////////////

//���ݿ���
class CDataBaseEngineSink : public IDataBaseEngineSink, public IGameDataBaseEngine
{
	//��Ԫ����
	friend class CServiceUnits;

	//��Ϸ���ݿ�
protected:
	CDataBaseAide					m_GameDBAide;						//��Ϸ����
	CDataBaseHelper					m_GameDBModule;						//��Ϸ����

	//��Ϸ�����ݿ�
protected:
	CDataBaseAide					m_TreasureDBAide;					//�Ƹ����ݿ�
	CDataBaseHelper					m_TreasureDBModule;					//�Ƹ����ݿ�

	//ƽ̨���ݿ�
protected:
	CDataBaseAide					m_PlatformDBAide;					//ƽ̨���ݿ�
	CDataBaseHelper					m_PlatformDBModule;					//ƽ̨���ݿ�

	//���ñ���
protected:
	tagGameParameter *				m_pGameParameter;					//���ò���
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

	//�������
protected:
	IDataBaseEngine *				m_pIDataBaseEngine;					//����ӿ�
	IGameServiceManager *			m_pIGameServiceManager;				//�������

	//��ѯ�ӿ�
protected:
	IGameDataBaseEngineSink *		m_pIGameDataBaseEngineSink;			//���ݽӿ�

	//�������
public:
	IDBCorrespondManager *          m_pIDBCorrespondManager;            //����Э��

	//��������
public:
	//���캯��
	CDataBaseEngineSink();
	//��������
	virtual ~CDataBaseEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ò���
public:
	//��������
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//��������
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//��ȡ����
public:
	//��ȡ����
	virtual VOID * GetDataBase(REFGUID Guid, DWORD dwQueryVer);
	//��ȡ����
	virtual VOID * GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer);

	//���ܽӿ�
public:
	//Ͷ�ݽ��
	virtual bool PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize);

	//ϵͳ�¼�
public:
	//�����¼�
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//�����¼�
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����Ϣ��1
protected:
	
	//Sockt���ӹر�
	bool On_DBR_GP_QUIT(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	
	//ID ��¼
	bool On_DBR_Logon_UserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//ID��¼����
	bool On_DBO_Logon_UserID(DWORD dwContextID, const double &dLongitude, const double &dLatitude, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//����Ϣ��3
protected:
	//��������Ȧ����
	bool On_DBR_User_CreateGroupRoom(DWORD dwContextID, VOID *pData, WORD wDataSize, DWORD &dwUserID);
	//��������Ȧ���䷵��
	VOID On_DBO_User_CreateGroupRoom(DWORD dwContextID, BYTE RULE[LEN_PRIVATE_TABLE_RULE], DWORD dwErrorCode, LPCTSTR pszErrorString);

	//��������Ȧ����
	bool On_DBR_User_JoinGroupRoom(DWORD dwContextID, VOID *pData, WORD wDataSize, DWORD &dwUserID);
	//��������Ȧ���䷵��
	VOID On_DBO_User_JoinGroupRoom(DWORD dwContextID, DWORD dwGroupID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//ϵͳ����
protected:
	//��Ϸд��
	bool OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�뿪����
	bool OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��Ϸ��¼
	bool OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//���ػ���
	bool OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//���ض�������
	bool On_DBR_GR_LOAD_OFFLINE(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	
	//�û�Ȩ��
	bool OnRequestManageUserRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//��������
	bool OnRequestMatchFee(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//������ʼ
	bool OnRequestMatchStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������
	bool OnRequestMatchOver(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������
	bool OnRequestMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�˳�����
	bool OnRequestMatchQuit(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//�����Ѷ��޸�
	bool OnRequestRoomLevelModify(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�������ֵ�޸�
	bool OnRequestRoomControlValModify(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//д��ǰ���
	bool OnWriteCurrentStock(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

protected:
	//�����û��Ƹ���Ϣ
	bool On_DBR_ModifyUserTreasure(DWORD dwContextID, void * pData, WORD wDataSize, DWORD &dwUserID);

protected:
	//¼�񱣴�
	bool On_DBR_SaveGameRecord(DWORD dwContextID, void * pData, WORD wDataSize, DWORD &dwUserID);

	//��������
private:

	//������ݱ�����
	VOID OnClearDB();

protected: //Club����Ȧ
	//club����Ȧ������Ϣ
	bool On_DBR_CLUB_ROOM_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//club����Ȧ������Ϣ
	bool On_DBR_CLUB_TABLE_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//club����Ȧ�����Ϣ
	bool On_DBR_CLUB_PLAYER_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
protected: //��Ҵ��� ����
	//��Ҵ��� ������Ϣ
	bool On_DBR_HALL_GOLD_TABLE_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ҵ��� �����Ϣ
	bool On_DBR_HALL_GOLD_PLAYER_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize);

protected:
	//��ӿ�����Ϣ
	bool OnRequestAddTableInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ʼ����
	bool OnRequestStartTable(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnRequestEndTable(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����û�������
	bool OnRequestAddUserToTable(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ɾ�������û�
	bool OnRequestDeleteUserToTable(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//���¿�����Ϣ
	bool OnUpdateTableInfo(DWORD dwTableID);
	//�����ѿ�����Ϣ
	bool OnUpdateEndTableInfo(DWORD dwTableID);
	//����齱
	bool OnQueryLottery(DWORD dwContextID, void * pData, WORD wDataSize);

	//*********������Ϣ��¼����**********
	//������Ϸ����״̬
	bool On_Table_UpdateGameTaskStatus(DWORD dwContextID, void *pData, WORD wDataSize);


	//club����Ȧ��������
	bool On_DBR_CG_CLUB_CREATE_TABLE(DWORD dwContextID, void *pData, WORD wDataSize);

	//�������� ����Ҫ����
	bool On_DBR_CG_USER_JOIN_TABLE_NO_PASS(DWORD dwContextID, void *pData, WORD wDataSize);

	//�������� 
	bool On_DBR_CG_JOIN_TABLE(DWORD dwContextID, void *pData, WORD wDataSize);

	//�������� ��Ҵ�������
	bool On_DBR_CG_USER_JOIN_TABLE_HALL_GOLD(DWORD dwContextID, void *pData, WORD wDataSize);
};

// byte����תΪ string  TODONOW ��ʱ�������ﴦ��
const CString toHexString(const byte * input, const int datasize);

// string תΪbyte����  TODONOW ��ʱ�������ﴦ��
int StrToBin(TCHAR* inWord, BYTE* OutBin, int source_len_begin, int source_len_end);

//////////////////////////////////////////////////////////////////////////////////

#endif