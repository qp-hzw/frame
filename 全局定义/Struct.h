#ifndef STRUCT_HEAD_FILE
#define STRUCT_HEAD_FILE

#include "Define_base.h"

#pragma region ��Ϸ�б�  -- ��Դ: ���ݿ�
//TypeItem
struct tagGameType
{
	WORD							wTypeID;							//��������
	TCHAR							szTypeName[LEN_TYPE];				//��������
};

//KindItem
struct tagGameKind
{
	WORD							wKindID;							//��������
	WORD							wTypeID;							//��������
	TCHAR							szKindName[LEN_KIND];				//��Ϸ����
};

//NodeItem
struct tagGameNode
{
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	TCHAR							szNodeName[LEN_NODE];				//�ڵ�����
};

//GameItem
struct tagGameGame
{
	DWORD							wGameID;							//ģ���ʶ
	TCHAR							szGameName[LEN_KIND];				//��Ϸ����
	TCHAR							szGameServerAddr[LEN_DB_ADDR];		//��Ϸ����ַ
	TCHAR							szDLLName[LEN_DLL];					//DLL����

	//�����ݿ�, ������ʹ��
	//TODONOW ���ֶ��Ǹ���szDLLName���жϵ�ǰ�Ƿ���dll. ��������ڴ˴�, ÿ�ζ���Ҫʹ�ú�����getһ��
	DWORD							dwNativeVersion;					//У��Dll�Ƿ����
};

//RoomItem
struct tagGameRoomItem
{
	//��������
	DWORD							dwServerID;							//�����ʶ
	TCHAR							szServerName[LEN_SERVER];			//��������

	WORD							wGameServerPort;					//��Ϸ���˿�

	//�����ݿ�, ������ʹ��
	//TODONOW DLLName���ֶ���ʵ�Ǵ���Database�е�GameItem�е� ��������ڴ˴�, �����о��޷�����dll
	//        dwNativeVersionԭ���� ����Ҫÿ��У��dll�Ƿ����
	TCHAR							szDLLName[LEN_DLL];					//DLL����
	DWORD							dwNativeVersion;					//У��Dll�Ƿ����
	//GameItem���ж�ȡ
	TCHAR							szGameServerAddr[LEN_DB_ADDR];		//��Ϸ����ַ
};

#pragma endregion

#pragma region ��Ϸ��, Э����, ��¼��, �ͻ��� ͳһ����
//��Ϸ���� -- ����client��
struct tagGameServer
{
	DWORD							wServerID;							//��������
	TCHAR							szServerName[LEN_SERVER];			//��������
	
	TCHAR							szGameServerAddr[LEN_IP_ADDR];		//��Ϸ����ַ
	WORD							wGameServerPort;					//��Ϸ���˿�

	DWORD							dwSubGameVersion;					//����Ϸ�汾��Ϣ

	DWORD							dwOnlineCount;						//��������ͳ��

	BYTE							byMask;								//����ɾ����־ 0Ϊ���� 1λɾ��
};

#pragma endregion

//������Ϣ
struct tagOnLineInfoGame
{
	DWORD							dwGameID;							//���ͱ�ʶ
	DWORD							dwOnLineCount;						//��������
};

//////////////////////////////////////////////////////////////////////////////////
//�û���Ϣ

//����״̬
struct tagTableStatus
{
	BYTE							cbTableLock;						//������־ TODONOW delete this
	BYTE							cbPlayStatus;						//��Ϸ��־ TODONOW delete this
};

//�û�״̬
struct tagUserStatus
{
	WORD							wTableID;							//��������
	WORD							wChairID;							//����λ��
	BYTE							cbUserStatus;						//�û�״̬
};

//�û�����
struct tagUserAttrib
{
	BYTE							cbCompanion;						//�û���ϵ
};

//�û�����
struct tagUserScore
{
	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lInsure;							//�û�����
	SCORE							lOpenRoomCard;						//�û�����

	//��Ӯ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������

	//ȫ����Ϣ
	DWORD							dwUserMedal;						//�û�����
	DWORD							dwExperience;						//�û�����
	LONG							lLoveLiness;						//�û�����
};

//�û�����
struct tagMobileUserScore
{
	//������Ϣ
	SCORE							lScore;								//�û�����

	//��Ӯ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������

	//ȫ����Ϣ
	DWORD							dwExperience;						//�û�����
};

//����ʹ��
struct tagUsePropertyInfo
{
	WORD                            wPropertyCount;                     //������Ŀ
	WORD                            dwValidNum;						    //��Ч����
	DWORD                           dwEffectTime;                       //��Чʱ��
};

//�û�����
struct tagUserProperty
{
	WORD                            wPropertyUseMark;                   //���߱�ʾ
	tagUsePropertyInfo              PropertyInfo[MAX_PT_MARK];			//ʹ����Ϣ   
};

//���߰���
struct tagPropertyPackage
{
	WORD                            wTrumpetCount;                     //С������
	WORD                            wTyphonCount;                      //��������
};

//�û���Ϣ
struct tagUserInfo
{
	//��������
	DWORD							dwUserID;							//�û� I D
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	DWORD							dwGroupID;							//���� I D
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������
	TCHAR							szUnderWrite[LEN_MY_SIGNATURE];		//����ǩ��

	//ͷ����Ϣ
	DWORD							dwCustomID;							//�Զ���ʶ
	TCHAR							szHeadUrl[LEN_HEAD_URL];			//ͷ���ַ

	//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							cbMasterOrder;						//����ȼ�

	//�û�״̬
	WORD							wTableID;							//��������
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬

	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lControlScore;						//�û�����ֵ
	SCORE							lOpenRoomCard;						//��������
	SCORE							lDiamond;							//��ʯ
	SCORE							lGold;								//���

	//��Ϸ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������
	DWORD							dwUserMedal;						//�û�����
	DWORD							dwExperience;						//�û�����
	LONG							lLoveLiness;						//�û�����

	//GPS added by lizhihu TODO ��ʱ�����������ͳһ
	double							dLongitude;							//����
	double							dLatitude;							//γ��
};

//////////////////////////////////////////////////////////////////////////////////

//�㳡����
struct tagGamePlaza
{
	WORD							wPlazaID;							//�㳡��ʶ
	TCHAR							szServerAddr[32];					//�����ַ
};

//��������
struct tagLevelItem
{
	LONG							lLevelScore;						//�������
	TCHAR							szLevelName[16];					//��������
};

//��Ա����
struct tagMemberItem
{
	BYTE							cbMemberOrder;						//�ȼ���ʶ
	TCHAR							szMemberName[16];					//�ȼ�����
};

//��������
struct tagMasterItem
{
	BYTE							cbMasterOrder;						//�ȼ���ʶ
	TCHAR							szMasterName[16];					//�ȼ�����
};

//�б�����
struct tagColumnItem
{
	BYTE							cbColumnWidth;						//�б���
	BYTE							cbDataDescribe;						//�ֶ�����
	TCHAR							szColumnName[16];					//�б�����
};

//��ַ��Ϣ
struct tagAddressInfo
{
	TCHAR							szAddress[32];						//�����ַ
};

//��Ȩ��Ϣ
struct tagLicenseInfo
{
	TCHAR						szGuestName[30];						//�ͻ���(�����пո�)
	TCHAR						szExpire[20];							//����ʱ��
	TCHAR						szCode[MAX_PATH];						//��Ȩ��
	TCHAR						szMac[20];								//MAC
	TCHAR						szSystemInstall[MAX_PATH];				//
	TCHAR						szTimes[20];							//
	TCHAR						szCPUID[MAX_PATH];							//
};

//��������
struct tagServerOptionInfo
{
	//�ҽ�����
	WORD							wKindID;							//�ҽ�����
	WORD							wNodeID;							//�ҽӽڵ�
	WORD							wSortID;							//���б�ʶ

	//��������
	TCHAR							szServerName[LEN_SERVER];			//��������
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif