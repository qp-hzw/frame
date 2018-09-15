#ifndef STR_CMD_CORRESPOND_HEAD_FILE
#define STR_CMD_CORRESPOND_HEAD_FILE

#pragma pack(1)

#pragma region  MDM_REGISTER ע��ģ��
//ע���¼��
struct STR_CPR_LP_REGISTER_LOGON
{
	TCHAR							szServerAddr[32];					//��½�����ַ
};

//ע����Ϸ��
struct STR_CPR_GP_REGISTER_GAME
{
	DWORD							dwServerID;							//��������
	TCHAR							szServerName[LEN_SERVER];			//��������

	TCHAR							szGameServerAddr[32];				//��Ϸ����ַ
	WORD							wGameServerPort;					//��Ϸ���˿�
	
	DWORD							dwSubGameVersion;					//����Ϸ�汾
};

//ע��ʧ��
struct STR_CPO_PGPL_REGISTER_FAILURE
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

#pragma endregion

#pragma region MDM_CPD_LIST �б�����
//��������
struct STR_CPR_GP_LIST_GAME_ONLINE
{
	DWORD							dwOnLineCount;						//��������
};

//��������
struct STR_CPR_PL_LIST_GAME_ONLINE
{
	DWORD							wServerID;							//�����ʶ
	DWORD							dwOnLineCount;						//��������
};

//����ɾ��
struct STR_CPO_PL_LIST_REMOVE_GAME
{
	DWORD							dwServerID;							//�����ʶ
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
};
#pragma endregion

#pragma region  ���״̬

#pragma endregion 

#pragma region MDM_WEB ��̨����
//�������Ϣ
struct STR_CPR_WP_WEB_MARQUEE
{
	BYTE							byMask;								//��־
};

//����Ƶ�� -- ϵͳ��Ϣ
struct STR_CPR_WP_WEB_SYSTEM_MESSAGE
{
	TCHAR							szMessage[50];						//����Ƶ�� -- ϵͳ��Ϣ
};
#pragma endregion
#pragma pack()

#endif