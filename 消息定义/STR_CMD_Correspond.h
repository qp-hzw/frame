#ifndef STR_CMD_CORRESPOND_HEAD_FILE
#define STR_CMD_CORRESPOND_HEAD_FILE

#pragma pack(1)

#pragma region  MDM_REGISTER ע��ģ��
//��Ϸ����
struct tagServerItem
{
	DWORD							dwServerID;							//server id
	DWORD                           dwSocketID;                         //socket id
	BYTE                            byServerType;                       //1.lobbyServer;  2.GameServer
	TCHAR							szServerName[LEN_SERVER];			//
	
	TCHAR							szServerAddr[LEN_IP_ADDR];		    //��������ַ
	WORD							wServerPort;					    //�������˿�
};

//��������
enum enServerType
{
	NULL_TYPE = 1,				//��Ч����
	GAME_TYPE = 2,				//��Ϸ��
	LOBBY_TYPE = 3,				//��¼��
};

//ע��ʧ��
struct STR_CPO_PGPL_REGISTER_FAILURE
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

#pragma endregion

#pragma region MDM_TRANSFER ��ת����
//��¼��֪ͨЭ����, Э����֪ͨ��Ϸ�� ��ɢ����
struct STR_CPR_LP_CLUB_TABLE_DISSOLVE
{
	DWORD	dwTableID;									//����ID
	DWORD	dwGameID;									//��ϷID
};

//��¼��֪ͨЭ����, Э����֪ͨ��Ϸ�� ��ɢ����
struct STR_CPO_PG_CLUB_TABLE_DISSOLVE
{
	DWORD	dwTableID;									//����ID
	DWORD	dwGameID;									//��ϷID
};

//�������� ��ѯ���õ�GameID
struct STR_CPR_LP_CREATE_TABLE
{
	DWORD	dwKindID;									//TODONOW ȷ��kindID������

	DWORD	dwSocketID;									//�ͻ���SocketID
};

//�������� ��ѯ���õ�GameID
struct STR_CPO_PL_CREATE_TABLE
{
	//�������
	DWORD                           dwResultCode;							//�����ʶ
	TCHAR							szDescribeString[LEN_MESSAGE_DESCRIBE];	//������Ϣ

	DWORD	dwGameID;				//���õ�GameID;

	DWORD	dwSocketID;									//�ͻ���SocketID

	TCHAR	szIPAddr[LEN_IP_ADDR];					//ip��ַ
	DWORD	dwPort;									//ip�˿�
};

//��ѯ�������
struct STR_CPR_LP_OFFLINE_PLAYERQUERY
{
	DWORD  dwUserID;                               //�������
};

//��ѯ�������
struct STR_CPR_PG_OFFLINE_PLAYERQUERY
{
	DWORD  dwSocketID;                             //��ѯ��logon socket
	DWORD  dwUserID;                               //��ѯ�������
};

//��ѯ������� ����
struct STR_CPO_GP_OFFLINE_FINISH
{
	DWORD  dwSocketID;                             //��ѯ��logon socket
	DWORD  dwUserID;                               //��ѯ�������
	BYTE   bOffline;                               //0��������û�и����; 1���������и����
};

//��ѯ������ҷ���
struct STR_CPO_PL_OFFLINE_FiNISH
{
	DWORD  dwSocketID;                             //��ѯ��logon socket
	DWORD  dwUserID;                               //��ѯ�������
	DWORD  dwGameID;                               //GameID
};

#pragma endregion

#pragma region MDM_TRANSFER �û�
//��������
struct tagOfflineUser 
{
	DWORD dwUserID;
	DWORD dwServerID;	
	BYTE  byMask;		//1��ʾ��list�������û���  2��ʾ��list��ɾ���û�
};

#pragma endregion

#endif