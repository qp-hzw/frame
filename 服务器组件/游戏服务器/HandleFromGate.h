#ifndef HANDLE_FROM_CLIENT_HEAD_FILE_L
#define HANDLE_FROM_CLIENT_HEAD_FILE_L

#include "Stdafx.h"
#include "DataBasePacket.h"
#include "TableFrame.h"

class CHandleFromGate
{
	//�ַܷ�����
public:
	static bool HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	static bool HandlePacketDB(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize);

	//һ���ַ�����
private:
    //��¼���� MDM_LOGON
	static bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û����� MDM_USER
	static bool OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ܴ��� MDM_FRAME
	static bool OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
    //��Ϸ���� MDM_GAME
	static bool OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������� MDM_MATCH
	static bool OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//���崦��
private:
	#pragma region ��¼ģ�� MDM_LOGON
	//I D ��¼
	static bool On_SUB_CG_Logon_UserID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ID��¼����
	static bool On_CMD_GC_Logon_UserID(DWORD dwContextID, VOID * pData, WORD wDataSize);

    //��¼ģ�� ��������
private:
	//ID��¼�ɹ��������û���Ϊ�˸�pCPlayer��ֵ������ȥ˫ָ�룬pCPlayer����ȥ�ľ���NULL��
	static void ActiveUserItem(CPlayer **pCPlayer, DWORD dwContextID, 
		STR_DBO_CG_LOGON_USERID *pDBOLogon);

	//�û���¼
	static VOID OnEventUserLogon(CPlayer * pCPlayer, bool bAlreadyOnLine);
	//�û��ǳ�
	static VOID OnEventUserLogout(CPlayer * pCPlayer, DWORD dwLeaveReason);

	//�л�����
	static bool SwitchUserItemConnect(CPlayer *pCPlayer, TCHAR szMachineID[LEN_MACHINE_ID],
								const double &dLongitude, const double &dLatitude, 
								BYTE cbDeviceType=DEVICE_TYPE_PC, WORD wBehaviorFlags=0, WORD wPageTableCount=0);
#pragma endregion

	#pragma region  �û����� MDM_USER
	// �û����� MDM_USER
protected:
	//��������
	static bool On_SUB_CG_USER_CREATE_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������(���� ����Ȧ)
	static bool On_SUB_CG_USER_CREATE_TABLE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������� --��Ҫ����
	//��Ҫ���ݿ�У�� 1. ����ķ����� �Ƿ��㹻
	//				 2. �ǹ����Ա���ܽ��빤������
	static bool On_SUB_User_JoinFkRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������� -- ����Ҫ����
	//��Ҫ���ݿ��жϲ����ؿɼ�������Ӻ�
	static bool On_SUB_CG_USER_JOIN_TABLE_NO_PASS(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	
	//�������� -- ���������ҳ�����
	//��Ҫ���ݿ��жϲ����ؿɼ�������Ӻ�, ���û����ᴴ��һ���µ�����
	static bool On_SUB_CG_USER_JOIN_GOLD_HALL_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���뷿�� -- ���������ҳ����� ����
	static bool On_CMD_GC_USER_JOIN_TABLE_HALL_GOLD( DWORD dwContextID, VOID * pData, WORD wDataSize );
	

	//��������
	static bool On_SUB_User_ReconnectRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����ҷ�
	static bool On_SUB_User_JoinGoldRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ɢ����
	static bool On_SUB_User_DismissRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������Ȧ����
	static bool On_Sub_CG_User_CreateGroupRoom(VOID *pData, WORD wDataSize, DWORD dwSocketID);
	//��������Ȧ���䷵��
	static bool On_CMD_GC_User_CreateGroupRoom(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��������Ȧ����
	static bool On_Sub_CG_User_JoinGroupRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������Ȧ���䷵��
	static bool On_CMD_GC_User_JoinGroupRoom(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�޸��û��Ƹ���Ϣ
	static bool On_CMD_GC_User_ModifyUserTreasure(DWORD dwContextID, VOID * pData, WORD wDataSize);


	//�û�����
	static bool On_SUB_CG_User_SitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	static bool On_SUB_CG_User_StandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����û�
	static bool On_SUB_CG_User_InviteUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�ܾ��������
	static bool On_SUB_CG_User_RefuseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�߳����û�
	static bool On_SUB_CG_User_KickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�������λ��
	static bool OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//���󷿼��б�
	static bool OnTCPNetworkSubGetTableList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����齱
	static bool OnTcpNetworkQueryLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID);

    //�齱���
	static bool OnDBLotteryResult( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//Club����Ȧ��������
	static bool On_CMD_CG_CLUB_CREATE_TABLE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//���뷿��  ����Ҫ����
	static bool On_CMD_GC_USER_JOIN_TABLE_NO_PASS( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//���뷿��  ��Ҫ���� -- ���ݿ�У��
	static bool On_CMD_GC_JOIN_TABLE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
#pragma endregion

	#pragma region ������� MDM_FRAME
	//������� MDM_FRAME
protected:
	//�����ɢ����
	static bool On_SUB_Common_ApplyDismissRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ƿ��ɢ����
	static bool On_SUB_Common_VoteDismissRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
#pragma endregion

	#pragma region ������
	//�����¼�
protected:
	//���ض�������
	static bool On_DBO_GR_LOAD_OFFLINE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//���ͺ���
protected:
	//�����û���Ϣ
	static bool SendUserInfoPacket(CPlayer * pCPlayer, DWORD dwSocketID);

	//��������
protected:
	//����ʧ��
	static bool SendRequestFailure(CPlayer * pCPlayer, LPCTSTR pszDescribe, LONG lErrorCode);

	//��������
public:
	//���û�
	static CPlayer * GetBindUserItem(WORD wBindIndex);

	//��������
protected:
	//Ⱥ������
	static bool SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
#pragma endregion

#pragma region  �������� && �������� && ��������
    //�������� && ��������
protected: 
	//��鴴�����ӵ���Ʊ
	static bool CheckCreateTableTicket(tagTableRule * pCfg, CPlayer *pCPlayer);

    //������ͨ����(1. Ϊ�Լ���������  2.Ϊ���˴�������)
	static bool CreateTableNormal(tagTableRule * pCfg, CPlayer *pCPlayer, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom);
	//��������Ȧ����
	static bool CreateRoomClub(tagTableRule * pCfg, CPlayer *pCPlayer, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom);
	//��������Ȧ����(�ȶ�ȡ���ݿ��еķ������)
	static bool CreateTableClub(STR_DBO_GC_CLUB_CREATE_TABLE* , CPlayer *pCPlayer);

	//�������� ��Ҵ�������  -- ��ҽ����Ҵ���ʱ��, ���û���ҵ����õ�����,�ͻᴴ������
	static bool CreateTableHallGold(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD* , CPlayer *pCPlayer);

	//�������� ���ֲ�����  -- ��ҽ�����ֲ�����ʱ��, ���û���ҵ����õ�����,�ͻᴴ������
	static bool CreateTableAutoClub(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS* , CPlayer *pCPlayer);



    //�������� && �������� ��������
protected: 
    //Ϊ�Լ���������
	static bool HandleCreateTable(CTableFrame *pCurTableFrame, CPlayer *pCPlayer, tagTableRule *pCfg);
	//Ϊ���˴�������
	static void HandleCreateTableForOthers(CTableFrame *pCurTableFrame, CPlayer *pCPlayer, tagTableRule *pCfg);

	//����Ȧ������Ϣ��Ҫд�����ݿ�
	static bool WriteClubRoomToDB(STR_DBR_CLUB_ROOM_INFO* pTableInfo);

    //Ѱ����һ��������
	static CTableFrame* GetNextEmptyTable();
    //��������ID�������뷿������
	static DWORD GenerateTablePassword();

    //��������Ѱ��ָ������
	static CTableFrame* GetDesignatedTable(const DWORD &dwPassword);

    //���ҽ�ҷ�������
	static CTableFrame* GetGlodRoomEmptyChair(WORD &wChairID);

	//��������
protected:
	 //���������ӵ���Ʊ
	static bool CheckJoinTableTicket(tagTableRule * pCfg, CPlayer *pCPlayer);

#pragma endregion 

#pragma region DB�¼�֪ͨ
	//�����˿���
public:
	//��������˿���
	static bool AddOtherRoomInfo(DBR_GR_UpdateTableInfo* pTableInfo);
	//�����˿�������  && club ��Ϸ��ʼ
	static  void ConcludeTable(DWORD dwTableID);
	//�����˿�����ʼ && clubС�ֽ���
	static  void StartTable(DWORD dwTableID);
	//�û����������˿��� && club���ӽ�ɢ
	static  void JoinTable(DWORD dwTableID, DWORD dwUserID);
	//�û��뿪�����˿���
	static  void LeaveTable(DWORD dwTableID, DWORD dwUserID);


	//Club����Ȧ2  &&  ��Ҵ�����1.�û���Ϣ  2.������Ϣ��
public:
	//�û�����
	static void ClubPlayerSitDown(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold);
	//�û�����
	static void ClubPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold);
	//���һ���û�����
	static void ClubLastPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold);
	
	//�������� -- ���ֲ�
	static void ClubTableCreate(DWORD dwClubRoomID, DWORD dwUserID, DWORD dwTableID, DWORD dwLockState);
	//�������� -- ��Ҵ���
	static void HallTableCreate(DWORD dwUserID, DWORD dwKindID,BYTE byGameType, DWORD dwTableID);
	//���俪ʼ��Ϸ
	static void ClubTableStart(DWORD dwTableID, BYTE byClubOrHallGold);
	//������������
	static void ClubTableMax(DWORD dwTableID, BYTE byClubOrHallGold);
	//�������� ����
	static void ClubTableNotMax(DWORD dwTableID, BYTE byClubOrHallGold);
	//С�ֽ�����Ϸ -- ���ֲ�
	static void ClubTableXJ(DWORD dwTableID);
	//��ֽ�����Ϸ -- ���ֲ�
	static void ClubTableDJ(DWORD dwTableID);

#pragma endregion

		//�û��ӿ�
public:
	//�����û��Ƹ���Ϣ
	static bool OnEventModifyUserTreasure(CPlayer *pCPlayer, DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin);
};

#endif