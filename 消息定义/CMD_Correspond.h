#ifndef CMD_CORRESPOND_HEAD_FILE
#define CMD_CORRESPOND_HEAD_FILE

#include "STR_CMD_Correspond.h"
#pragma pack(1)

/*
** ��Ҫ����:  Э���� �� ��������(��¼��, ��Ϸ��, web��̨) ��Ϣ
** ��ϸ����:  1. ��Ϣ��(�� + ��)     2. �ṹ��
** ��ע˵��:  ����Ϣ�ŵĺ���: 
**            1. ��һ���ֶ�: ������־,        
**				 CPR(��������->Э��������Ϣ)�� CPO(Э����->�������̵���Ϣ), 
**            2. �ڶ����ֶ�: ��Ϣ����         
**				 L: logon;  G:gameserver; W:web;  P:Э����
**            3. �������ֶ�: ����Ϣ�ŵı�־   ����REGISTER��Ϊע��ģ�����Ϣ
**            4. ֮����ֶ�: ����Ϣ�ŵĺ���   ����GameServer��ʾ��Ϸ��ע��
**           
**            �����淶
**            1. �ṹ�������:  ����Ϣ�ŵ�ǰ���STR, ����SUB_CL_LOGON_ACCOUNTS ��Ϊ STR_SUB_CL_LOGON_ACCOUNTS
**
**            2. �����������淶: 1) ָ���p  2)������־CPR  3)����Ϣ�ű�־  4)����Ϣ�ű�־
**
**            3. ����������:  On_������־_����Ϣ������Ϣ��
**
**
*/

#pragma region MDM_REGISTER ע��ģ��
//ע������
#define MDM_REGISTER				1									//ע��ģ��

//����ע��
#define CPR_LP_REGISTER_LOGON		1									//ע���¼��
#define CPR_GP_REGISTER_GAME		2									//ע����Ϸ��

//ע����
#define CPO_PGPL_REGISTER_SUCESS	101									//ע�����
#define CPO_PGPL_REGISTER_FAILURE	102									//ע��ʧ��

#pragma endregion

#pragma region MDM_CPD_LIST �б�����
#define MDM_CPD_LIST				2									//������Ϣ

#define CPR_GP_LIST_GAME_ONLINE		1									//��Ϸ�� �������� 
#define CPO_PL_LIST_GAME_ONLINE		101									//��Ϸ�� ��������

#define CPO_PL_LIST_INSERT_GAME		102									//������Ϸ��GameID(����) -- �ṹ��ΪtagGameServer
#define CPO_PL_LIST_INSERT_GAME_LIST 103								//������Ϸ��GameID(�б�) -- �ṹ��ΪtagGameServer
#define CPO_PL_LIST_REMOVE_GAME		104									//ɾ����Ϸ��GameID


//TODONOW added by WangChengQing 2018/9/6 û�п�������ʲô��
#define SUB_CS_S_SERVER_INFO		105									//������Ϣ
#pragma endregion

#pragma region MDM_TRANSFER ��ת����
#define MDM_TRANSFER				3									//��ת����

#define CPR_GP_CLUB_TABLE_INFO		1									//�㲥������Ϣ
#define CPO_PL_CLUB_TABLE_INFO		101									//�㲥������Ϣ

#define CPR_GP_CLUB_PLAYER_INFO		2									//�㲥�û���Ϣ
#define CPO_PL_CLUB_PLAYER_INFO		102									//�㲥�û���Ϣ

#define CPR_LP_CLUB_TABLE_DISSOLVE	3									//��ɢ����
#define CPO_PG_CLUB_TABLE_DISSOLVE	103									//��ɢ����

#define CPR_LP_CREATE_TABLE			4									//�������� ��ѯ���õ�GameID
#define CPO_PL_CREATE_TABLE			104									//�������� ��ѯ���õ�GameID

#pragma endregion 

#pragma region MDM_CS_USER_COLLECT �û�����
//TODONOW ��Ƕ���е��� added by WangChengQing 2018/9/6
#define MDM_CS_USER_COLLECT			4									//�û�����

//�û�״̬
#define SUB_CS_C_USER_ENTER			1									//�û�����
#define SUB_CS_C_USER_LEAVE			2									//�û��뿪
#define SUB_CS_C_USER_FINISH		3									//�û����
#define SUB_CS_C_USER_OFFLINE		4									//�û�����  game->correspond
// TODONOW ��¼��û�� ����Ϣ��3�Ĵ���, ��������Ϣ�ŵ�����Ϣ��Ϊ2 ֮���޸�
#define SUB_CS_C_USER_OFFLINE_B		5									//�û�����  correspond -> logon

//�û�״̬
#define SUB_CS_S_COLLECT_REQUEST	100									//��������

//////////////////////////////////////////////////////////////////////////////////
//��������
struct tagOfflineUser 
{
	DWORD dwUserID;
	DWORD dwServerID;	
	BYTE  byMask;		//1��ʾ��list�������û���  2��ʾ��list��ɾ���û�
};

//�û�����
struct CMD_CS_C_UserEnter
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�

	//������Ϣ
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							cbMasterOrder;						//����ȼ�

	//SCORE							lDiamond;							//��ʯ
	//SCORE							lGold;								//���
};

//�û��뿪
struct CMD_CS_C_UserLeave
{
	DWORD							dwUserID;							//�û���ʶ
};

struct STR_SUB_CS_C_USER_OFFLINE
{
	DWORD dwUserID;
	DWORD dwGameID;
	BYTE  byMask;		//1��ʾ��list�������û���  2��ʾ��list��ɾ���û�
};


#pragma endregion

#pragma region MDM_WEB ��̨����
#define MDM_WEB						5									//��̨����

#define CPR_WP_WEB_MARQUEE			1									//�������Ϣ (WEB --> CPD)
#define CPO_PL_WEB_MARQUEE			101									//�������Ϣ��CPD --> Logon��

#define CPR_WP_WEB_SYSTEM_MESSAGE	2									//����Ƶ�� -- ϵͳ��Ϣ��WEB --> CPD��
#define CPO_PL_WEB_SYSTEM_MESSAGE	102									//����Ƶ�� -- ϵͳ��Ϣ (CPR --> Logon)

#pragma endregion

#pragma pack()


#endif