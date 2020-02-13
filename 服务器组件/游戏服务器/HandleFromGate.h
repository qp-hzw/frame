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
	//����
	static bool OnTCPNetworkMainClub(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//���崦��
private:
	#pragma region ��¼ģ�� MDM_LOGON
	//I D ��¼
	static bool On_SUB_CG_Logon_UserID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ID��¼����
	static bool On_CMD_GC_Logon_UserID(DWORD dwContextID, VOID * pData, WORD wDataSize);

#pragma endregion

	#pragma region  �û����� MDM_USER
	// �û����� MDM_USER
protected:
	//��ɢ����
	static bool On_SUB_User_DismissRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������Ȧ���䷵��
	static bool On_CMD_GC_User_JoinGroupRoom(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�޸��û��Ƹ���Ϣ
	static bool On_CMD_GC_User_ModifyUserTreasure(DWORD dwContextID, VOID * pData, WORD wDataSize);


	//�û�����
	static bool On_SUB_CG_User_SitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	static bool On_SUB_CG_User_StandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�׼��
	static bool On_SUB_CG_User_Ready(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����û�
	static bool On_SUB_CG_User_InviteUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�߳����û�
	static bool On_SUB_CG_User_KickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�������λ��
	static bool OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//���������ɢ����
	static bool On_SUB_RG_USER_ASK_DISMISS(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����ɢ����
	static bool On_SUB_RG_USER_VOTE_DISMISS(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//���뷿��  ����Ҫ����
	static bool On_CMD_GC_USER_JOIN_TABLE_NO_PASS( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//�����ҳ�������Ϣ
	static bool On_SUB_CG_USER_GOLD_INFO(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	
	
#pragma endregion

	#pragma region ������
	//�����¼�
protected:
	//��������
protected:
	//����ʧ��
	static bool SendRequestFailure(CPlayer * pCPlayer, LPCTSTR pszDescribe, LONG lErrorCode);
#pragma endregion

	#pragma region  �������� && �������� && ��������
    //���̺���
public: 
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

	//���봴������
	static bool On_SUB_CG_USER_CREATE_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���÷������
	static bool On_SUB_CG_USER_SET_ROOM_RULE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	
	//�������� ���ֲ�����  -- ��ҽ�����ֲ�����ʱ��, ���û���ҵ����õ�����,�ͻᴴ������
	static bool CreateTableAutoClub(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS* , CPlayer *pCPlayer);

    //��������
protected: 
	//У�� ���봴������
	static bool CheckCreateRoom(CPlayer * player, BYTE gameMode);
	//��� �������ӵ���Ʊ
	static bool CheckCreateTableTicket(tagTableRule * pCfg, CPlayer *pCPlayer);

	//���������ӵ���Ʊ
	static bool CheckJoinTableTicket(tagTableRule * pCfg, CPlayer *pCPlayer);

	//����Ȧ������Ϣ��Ҫд�����ݿ�
	static bool WriteClubRoomToDB(STR_DBR_CLUB_ROOM_INFO* pTableInfo);
#pragma endregion 

	#pragma region DB�¼�֪ͨ

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

	#pragma region �����¼� MDM_GR_MATCH
private:
	//�����������Ϣ
	static bool On_SUB_CG_MATCH_INFO(VOID * pData, WORD wDataSize, DWORD dwSocketID);

#pragma endregion

#pragma region ����
	//��������
	static bool On_SUB_CG_CLUB_ROOM_CREATE(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//����÷����������Ϣ
	static bool On_SUB_CG_CLUB_TABLE_LIST_TABLE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	
#pragma endregion
		//�û��ӿ�
public:
	//�����û��Ƹ���Ϣ
	static bool OnEventModifyUserTreasure(CPlayer *pCPlayer, DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin);
};

#endif