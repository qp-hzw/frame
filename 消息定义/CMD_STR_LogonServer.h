#ifndef STR_CMD_LONGON_HEAD_FILE
#define STR_CMD_LONGON_HEAD_FILE

#include "Stdafx.h"

#pragma region �����ṹ��
////�����ϵ������Ϣ
struct tagClubTableUserInfo
{
	DWORD	dwUserID;									//���ID
	TCHAR	szUserName[LEN_NICKNAME];					//�������
	BYTE	bySex;										//�Ա�
	WORD	wLevel;										//�ȼ�
	TCHAR	szHeadUrl[LEN_URL];							//ͷ���ַ

	BYTE	byClubRole;									//���ְ�� 0 Ⱥ��; 1����; 2һ���Ա
	BYTE	byClubReputation;							//����ڹ����е������ȼ�
};

//��������
struct STR_CMD_LC_LIST_ROOM_ONLINE
{
	DWORD	dwServerID;						//����ID
	DWORD	dwOnlineCount;					//����
};

#pragma endregion

#pragma region MDM_LOGON ��¼ģ��
//�ʺŵ�¼  
struct STR_SUB_CL_LOGON_ACCOUNTS
{
	DWORD							dwVersionCheck;						//�汾У�飨32λ = ����汾(8λ) + �ں˰汾(8λ) + ƽ̨�汾(8λ) + ����Ϸ�汾(8λ)��
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	DWORD							dwProxyID;							//����ID
};
//ע���ʺ�
struct STR_SUB_CL_LOGON_REGISTER
{
	DWORD							dwVersionCheck;						//�汾У��
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�
	TCHAR                           strMobilePhone[LEN_MOBILE_PHONE];   //�ֻ����� ����Ϊnull
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	DWORD							dwProxyID;							//����ID
};
//ƽ̨��¼  
struct STR_SUB_CL_LOGON_PLATFORM
{
	DWORD							dwVersionCheck;						//�汾У��
	TCHAR							szOpenID[33];						//OpenID | tokenID
	TCHAR							szUnionID[33];						//UnionID
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�
	TCHAR							strHeadUrl[LEN_HEAD_URL];			//ͷ���ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	DWORD							dwProxyID;							//����ID
};

//�˺�|ƽ̨��½ ����
struct STR_CMD_LC_LOGON_PLATFORM
{
	//�������
	DWORD                           dwResultCode;							//�����ʶ
	//������Ϣ
	TCHAR							szDescribeString[LEN_MESSAGE_DESCRIBE];	//������Ϣ
	//�û���ʶ
	DWORD							dwUserID;							//�û���ʶ
	//��������
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szHeadUrl[LEN_HEAD_URL];			//ͷ���ַ
	TCHAR							szMySignature[LEN_MY_SIGNATURE];	//����ǩ��
	//��������
	SCORE                           dwUserDiamond;						//�û�Ԫ��
	DWORD							dwUserMedal;						//�û�����
	BYTE							byLevel;						//������ֵ
	DWORD							dwLoveLiness;						//�û�����
	//�û�Ȩ��
	BYTE                            cbMasterOrder;                      //����Ա�ȼ� 0��ʾ�ǹ���Ա
	BYTE							cbMemberOrder;						//��Ա�ȼ�   0��ʾ�ǹ���Ա
	SYSTEMTIME						MemberOverDate;						//��Ա����ʱ��
	TCHAR                           szIdentityName[LEN_IDENTITY_NAME];  //��ʵ����
	TCHAR                           szIdentityNum[LEN_IDENTITY_NUM];    //���֤��
	TCHAR                           szMobilePhone[LEN_MOBILE_PHONE];    //�ֻ�����	
	//�˺���Ϣ
	TCHAR							szLasLogonIp[LEN_IP_ADDR];			//����¼��ַ
	SYSTEMTIME						LasLogonDate;						//�������ʱ��
	//�Ƹ���Ϣ
	SCORE							lUserScore;							//�û���Ϸ��
	SCORE							lUserGold;							//�û����
	SCORE							lOpenRoomCard;						//�û�����

	//������Ϣ. 
	DWORD							dwOffLineGameID;					//����������GameID, �ǿձ�ʾ�Ƕ�������

	//TODO ���Ӵ���ID
};

//�ظ���¼
struct STR_CMD_LC_LOGON_REPEAT_LOGON
{
	TCHAR szDescribe[LEN_MESSAGE_DESCRIBE];                                              //��ʾ��Ϣ
};

//������ʾ
struct STR_CMD_LC_LOGON_UPDATE_NOTIFY
{
	BYTE							cbUpdateNotify;						//������ʾ		0�汾Ƥ��	1-��������		2-ǿ������
	DWORD							dwCurrentServerVersion;				//��ǰ�������汾
};

//��¼����
struct STR_CMD_LC_LOGON_LOGON_REWARD
{
	BYTE	byRewardType;				//��������
	DWORD	dwRewardCount;				//��������
	TCHAR	szDescribe[LEN_MESSAGE_DESCRIBE];			//����
};
#pragma endregion

#pragma region MDM_LIST �б�����
struct STR_SUB_CL_LIST_QUERY_GAMEID
{
	DWORD		kindID; //TODONOW �������� �ٶ�
};

struct STR_CMD_LC_LIST_QUERY_GAMEID
{
	DWORD		dwGameID;	//���õ�gameID
};
#pragma endregion

#pragma region MDM_SERVICE �û�����

//��ҷ���
struct STR_SUB_CL_SERVICE_FEEDBACK
{
	DWORD							dwUserID;								//�û�ID
	TCHAR							szFB_Title[LEN_FEEDBACK_TITLE];			//��������
	TCHAR							szFB_Content[LEN_FEEDBACK_CONTENT];		//��������
	TCHAR							szContact[LEN_MOBILE_PHONE];			//�绰����
};
//��ҷ�������
struct STR_CMD_LC_SERVICE_FEEDBACK
{
	LONG							lResultCode;								//�������
	TCHAR							szDescribeString[LEN_MESSAGE_DESCRIBE];		//��Ϣ����
};

//ˢ���û���Ϣ
struct STR_SUB_CL_SERVICE_REFRESH_INFO
{
	DWORD							dwUserID;							//�û�ID
};
//ˢ���û���Ϣ����
struct STR_CMD_LC_SERVICE_REFRESH_INFO
{
	DWORD                           dwResultCode;                       //�����ʶ
	TCHAR							szDescribeString[128];				//������Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�
	WORD							wFaceID;							//ͷ������
	TCHAR							szMySignature[LEN_MY_SIGNATURE];	//����ǩ��
	SCORE                           dwUserDiamond;						//�û�Ԫ��
	DWORD							dwUserMedal;						//�û�����
	DWORD							dwExperience;						//������ֵ
	DWORD							dwLoveLiness;						//�û�����
	BYTE                            cbMasterOrder;                      //����Ա�ȼ� 0��ʾ�ǹ���Ա
	BYTE							cbMemberOrder;						//��Ա�ȼ�   0��ʾ�ǹ���Ա
	SYSTEMTIME					MemberOverDate;						//��Ա����ʱ��
	TCHAR                           szIdentityName[LEN_IDENTITY_NAME];  //��ʵ����
	TCHAR                           szIdentityNum[LEN_IDENTITY_NUM];    //���֤��
	TCHAR                           szMobilePhone[LEN_MOBILE_PHONE];    //�ֻ�����
	TCHAR							szLasLogonIp[LEN_IP_ADDR];			//����¼��ַ
	SYSTEMTIME						LasLogonDate;						//�������ʱ��		TODO �ͻ���û���������ʱ�䣬У���ǲ��ǵ������ݰ���С
	DWORD							dwGroupID;							//���ű�ʶ
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������
	SCORE							lUserScore;							//�û�����
	SCORE							lUserGold;							//�û����
	SCORE							lUserRoomCard;						//�û�����
};

//������Ϣ�б�
struct STR_SUB_CL_SERVICE_QUERY_ROOMLIST
{
	DWORD dwUserID;
};
//������Ϣ�б���
struct STR_CMD_LC_SERVICE_QUERY_ROOMLIST
{
	DWORD dwTableID;			//����ID
	DWORD dwPassword;			//��������
	DWORD dwCreaterID;			//�������ID
	WORD  wJuShu;				//����
	BYTE  byMode;				//ģʽ
	BYTE  byZhuangType;			//ׯ����
	BYTE  byPlayerCount;		//�����
	BYTE  byMaxPlayerCount;		//��������
	BYTE  byIsStart;			//�Ƿ�ʼ
	BYTE  byCost;				//�ۿ���
	TCHAR szTime[24];			//����ʱ��
	DWORD dwUserID[5];			//���ID
	TCHAR szNickName[5][LEN_NICKNAME];		//����ǳ�
	DWORD dwScore[5];					//�û�����
};

//�޸ĸ�������	
struct STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO
{
	//�û���ʶ
	DWORD							dwUserID;							//�û�ID
	TCHAR							szOldLogonPassword[LEN_PASSWORD];	//�ɵĵ�½����
	//������Ϣ
	TCHAR							szNewLogonPassword[LEN_PASSWORD];	//�µĵ�½���루�����޸ģ��;ɵ�һ����
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�	
	TCHAR							szHeadImageUrl[LEN_HEAD_URL];			//ͷ���ַ
	TCHAR							szSignature[LEN_MY_SIGNATURE];		//����ǩ��
	//��ϵ��ʽ
	TCHAR							szRealName[LEN_IDENTITY_NAME];		//��ʵ����
	TCHAR							szIDCardNum[LEN_IDENTITY_NUM];		//���֤��
	TCHAR							szPhoneNum[LEN_MOBILE_PHONE];		//���ֻ���
	//��������
	DWORD							dwProxyUserID;						//�����û�ID		//TODO ����ID�����ݿ�����ʱδ���ӣ���������
};
//�޸ĸ������Ϸ���
struct STR_CMD_LC_SERVICE_MODIFY_PERSONL_INFO
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

//��ȡ������
struct STR_SUB_CL_SERVICE_GET_RICHLIST
{

};
//����������ṹ��
struct tagSingleRichInfo
{
	TCHAR szName[LEN_NICKNAME];				//�û���
	DWORD dwMoney;							//��Ǯ
	TCHAR szWechatAccount[LEN_ACCOUNTS];	//΢���˺�
	DWORD dwRegistID;						//��֤ID
	TCHAR szQQ[LEN_ACCOUNTS];				//QQ��
	TCHAR szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ���
};
//��ȡ�����񷵻�
struct STR_CMD_LC_SERVICE_GET_RICHLIST
{
	BYTE byCount;						//���а�����
	tagSingleRichInfo RegalInfoList[15];	//�����б�
};

//��ȡ�û�¼���б�
struct STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST
{
	DWORD dwUserID;
};
//��ȡ�û�¼���б���
struct STR_CMD_LC_SERVICE_GET_DJ_RECORDLIST
{
	DWORD							dwTableID;			//����ID
	DWORD							dwUserID[5];		//���ID
	TCHAR							szUserName[5][LEN_NICKNAME];//�û��ǳ�
	BYTE							byGameMode;			//��Ϸģʽ
	BYTE							byZhuangType;		//��ׯ����
	WORD							wCurrentJuShu;		//��ǰ����
	WORD							wTotalJuShu;		//�ܾ���
	TCHAR							szGameTime[30];		//��Ϸʱ��
	TCHAR							szTotalScore[50];	//��Ҵ���ܷ���
};

//��ȡС��¼���б�
struct STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST
{
	DWORD dwTableID;												//����ID
};
//��ȡС��¼���б���
struct STR_CMD_LC_SERVICE_GET_XJ_RECORDLIST
{
	WORD							wCurrentJuShu;					//��ǰ����
	TCHAR							szNickName[5][LEN_NICKNAME];	//�û��ǳ�
	TCHAR							szScore[50];					//���С�ַ���

	//TODO ��������Ӿ��������
};

//��ȡ���߽���
struct STR_SUB_CL_SERVICE_ONLINE_REWARD   
{
	DWORD dwUserID;
};
//��ȡ���߽�������
struct STR_CMD_LC_SERVICE_ONLINE_REWARD
{
	BYTE  byType;		//��������
	DWORD dwCount;		//��������
};

//��ȡ�����б�
struct STR_SUB_CL_SERVICE_GET_TASKLIST
{
	DWORD	dwUserID;				//�û�ID
};
//������Ϣ
struct tagTaskInfo
{
	WORD	wTaskID;							//����ID
	BYTE	cbTaskType;							//�������� 1�ճ�; 2�ܳ�; 3�³�; 4�ɾ�
	BYTE	cbActionType;						//���������� ��1����Ϣ; 2΢�ŷ���; 3��ֵ; 4������Ϸ; 5�����Ϸ	
	DWORD	dwNeedCount;						//����������������, �������5�ַ�����Ϸ, �������5	
	TCHAR	szDescribe[128];					//��������
	BYTE	byRewardType;						//��������1���; 2����; 3��ʯ
	DWORD	dwRewardCount;						//��������
	BYTE	cbTaskStatus;						//����״̬ ��1-������  1-��ɵ�δ��ȡ 2-�������ȡ
	DWORD	dwFinishedCount;					//����ɵ�������������
};
//��ȡ�����б���
struct STR_CMD_LC_SERVICE_GET_TASKLIST
{
	WORD	wTaskID;							//����ID
	BYTE	cbTaskType;							//�������� 1�ճ�; 2�ܳ�; 3�³�; 4�ɾ�
	BYTE	cbActionType;						//���������� ��1����Ϣ; 2΢�ŷ���; 3��ֵ; 4������Ϸ; 5�����Ϸ	
	DWORD	dwNeedCount;						//����������������, �������5�ַ�����Ϸ, �������5	
	TCHAR	szDescribe[128];					//��������
	BYTE	byRewardType;						//��������1���; 2����; 3��ʯ
	DWORD	dwRewardCount;						//��������
	BYTE	cbTaskStatus;						//����״̬ ��0-������  1-��ɵ�δ��ȡ 2-�������ȡ
	DWORD	dwFinishedCount;					//����ɵ���������
	//������Ϣ
	BYTE	cbListFinishMask;					//�����б�����ɱ�־ 0-δ��� 1-���
};

//��ȡ������
struct STR_SUB_CL_SERVICE_GET_TASK_REWARD
{
	DWORD	dwUserID;
	WORD	wTaskID;	
};
//��ȡ����������
struct STR_CMD_LC_SERVICE_GET_TASK_REWARD				
{
	LONG	lResultCode;												//0-�������  ����-����δ���
	TCHAR	szDescribe[128];											//������������� �����,���10000���
};

//��ȡ���а��б�
struct STR_SUB_CL_SERVICE_GET_RANKLIST
{
	BYTE	byIndex;  //�����,0�������ķ��� 1����ʤ�� 2�������� 3����ʤ�� 4�������� 5����ʤ�� 6�������� 7����ʤ��
};
//���а���Ϣ
struct tagRankInfo
{
	BYTE	byIndex;				//�����
	DWORD	dwUserID;				//�û�ID
	DWORD	dwCount;				//���ѻ�ʤ������
	BYTE	byRankNum;				//����
	BYTE	byReceived;				//����ȡ
	TCHAR	szNickName[LEN_NICKNAME];//�ǳ�
	TCHAR	szHeadUrl[LEN_HEAD_URL];	//ͷ��
};
//��ȡ���а��б���
struct STR_CMD_LC_SERVICE_GET_RANKLIST
{
	tagRankInfo rankItemInfo;
};

//����齱
struct STR_SUB_CL_SERVICE_REQUEST_LOTTERY
{
	DWORD	dwUserID;				//�û�ID
};
//����齱����
struct STR_CMD_LC_SERVICE_REQUEST_LOTTERY
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//������Ϣ
	BYTE							byIndex;							//ҡ����ţ�ת�̵ķ�����������0-11	
};

//pure�������а� ��ѯ
struct STR_SUB_CL_SERVICE_PURE_STANDING_LIST
{
	DWORD	dwUserID;				//�û�ID
};
//pure�������а� ����
struct STR_CMD_LC_SERVICE_PURE_STANDING_LIST
{
	BYTE	byRanking;					//���� ֻչʾǰ200���˵�
	DWORD	dwUserID;					//���id
	DWORD   dwLoveLiness;				//�û�����
	TCHAR	szNickName[LEN_NICKNAME];	//�û��ǳ�
	TCHAR	szHeadUrl[LEN_HEAD_URL];	//�û�ͷ��
};
//pure�������а� ����
struct STR_CMD_LC_SERVICE_PURE_STANDING_FINISH
{
	BYTE	byMask;						//����
	DWORD	dwRanking;					//�����ߵ����� ֻչʾǰ200���˵�
	DWORD   dwLoveLiness;				//�û�����
};

//��ȡ���а���
struct STR_SUB_CL_SERVICE_GET_RANK_REWARD
{
	BYTE	byIndex;
	DWORD	dwUserID;
};
//��ȡ���а�������   
struct STR_CMD_LC_SERVICE_GET_RANK_REWARD				
{
	LONG	lResultCode;												//0-��ȡ�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];											//������������� ��ȡ�ɹ�,���10000���
};

//��ѯ�û���ҷ�����Ϣ		TODO ��¼���б�ʱ�õ���ò�ƴ���	�����޸�Ϊ�ͻ��˴�����ˢ�°�ť
struct CMD_GP_QUERY_ScoreInfo
{
	DWORD							dwUserID;							//�û�ID
};
//�����û���ҷ�����Ϣ
struct STR_CMD_LC_QUERY_SCORE_INFO
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//������Ϣ
	DWORD							dwUserID;							//�û�ID
	LONGLONG						lOpenRoomCard;						//����
	LONGLONG						lGold;								//���
	LONGLONG						lDiamond;							//��ʯ
};

//���ս�� ��ѯ
struct STR_SUB_CL_SERVICE_PURE_RECORD_LIST
{
	DWORD						dwUserID;					//���ID
	DWORD						dwClubID;					//����Ȧ(�汾2)����, Ϊ0��ʾ�Ǵ���
	BYTE						byMask;						//1������  2��ҳ�
	SYSTEMTIME					tmQueryStartData;			//��Ҫ��ѯ�Ŀ�ʼ����(��С��λΪ��)
	//SYSTEMTIME					tmQueryEndData;				//��Ҫ��ѯ�Ľ�ֹ����(��С��λΪ��)
};
//���ս�� ����
struct STR_CMD_LC_SERVICE_PURE_RECORD_LIST
{
	BYTE						byMask;						//1������  2��ҳ�
	DWORD						dwDrawID;					//��ּ�¼ ����
	TCHAR						szKindName[32];				//��Ϸ��
	DWORD						dwTableID;					//����id
	TCHAR						szTime[128];				//��ʼʱ�� -- ����ʱ��  2018.4.16 14:00- 
	//SYSTEMTIME					szStartDataTim;				//��ʼʱ��(2018.4.16 12.00-13.55)
	//SYSTEMTIME					szEndDataTim;				//����ʱ��(2018.4.16 12.00-13.55)
};
//���ս�� ���������Ϣ
struct STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO
{
	DWORD					 dwDrawID;						//��ּ�¼ ����
	DWORD					 dwUserID;						//���ID
	TCHAR					 szNickName[32];				//�ǳ�
	TCHAR					 szHeadUrl[LEN_HEAD_URL];		//ͷ��
	SCORE					 lScore;						//��ҵ÷�
};
//���ս�� ����
struct STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST
{
	BYTE					byMask;							//������־
};

//С��ս�� ��ѯ
struct STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST
{
	DWORD					 dwDrawID;						//��ּ�¼ ����
};
//С��ս�� ����
struct STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST
{
	BYTE					bRoundCount;					//����
	DWORD					dwRecordID;						//¼��ID��Ψһ��ʶ��
};
//С��ս�� ���������Ϣ
struct STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO
{
	BYTE					 bRoundCount;					//��ּ�¼ ����
	DWORD					 dwUserID;						//���ID
	TCHAR					 szNickName[32];				//�ǳ�
	SCORE					 lScore;						//��ҵ÷�
};
//С��ս�� ����
struct STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH
{
	BYTE					byMask;						   //������־
};

//С��¼��ط�
struct STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK
{
	DWORD					dwRecordID;						//¼��ID
};
//С��¼��طŷ���
struct STR_CMD_LC_SERVICE_XJ_RECORD_PLAYBACK
{
	BYTE					cbfinish;							//���ͽ�����ʾ	0-δ����	1-����
	BYTE					cbRecordData[LEN_MAX_RECORD_SIZE/4];	//¼������
};

//�ͷ���ʾ��Ϣ
struct STR_SUB_CL_SERVICE_CUSTOMER_MESSEGE
{
	BYTE					cbMessegeFlag;						//��Ϣ��ʶ	0-��ʾ��Ϣ��ʾ	>=1��ʾ������Ϣ����
};
//�ͷ���ʾ��Ϣ ����
struct STR_CMD_LC_SERVICE_CUSTOMER_MESSEGE
{
	BYTE					cbMessegeFlag;						//��Ϣ��ʶ
	TCHAR					szMessege[LEN_MAX_SERVICE_MEG_SIZE];//��Ϣ����
};

//�����Ҵ�����Ϣ
struct STR_SUB_CL_SERVICE_GOLD_INFO
{
	DWORD					dwUserID;					   //���ID
	DWORD					dwModID;					   //��Ϸģʽ
	DWORD					dwKindID;					   //��Ϸ��KindID
};
//�����Ҵ�����Ϣ ����
struct STR_CMD_LC_SERVICE_GOLD_INFO
{
	BYTE					byGoldMod;						//1.����   2.�м�   3.�߼�   4.����
	DWORD					dwScore;						//�׷�
	DWORD					dwGold;							//�볡���
	DWORD					dwServiceGold;					//�볡����
};
//�����Ҵ�����Ϣ ����
struct STR_CMD_LC_SERVICE_GOLD_INFO_FINISH
{
	BYTE					byMask;						   //������־
};
 
#pragma endregion

#pragma region MDM_CLUB  ����Ȧ(�汾2)
#pragma region 1. ����Ȧ���
//��������Ȧ
struct STR_SUB_CL_CLUB_CREATE_CLUB
{
	DWORD	dwUserID;									//���ID
	TCHAR	szClubName[LEN_NICKNAME];					//����Ȧ����
	DWORD	dwMajorKindID;								//������ϷID
	DWORD	dwMinorKindID[10];							//��Ҫ��Ϸ ���10��

	TCHAR	szNotice[128];								//����Ȧ����
	TCHAR	szMessag[128];								//����Ȧ���
};

//��������Ȧ ����
struct STR_CMD_LC_CLUB_CREATE_CLUB
{
	LONG	lResultCode;								//0�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��

	DWORD	dwClubID;									//����ȦID
	BYTE	byClubLevel;								//����Ȧ�ȼ�
	//��̨�������� �´���������Ȧ��������
	DWORD	dwFK;										//����Ȧ�з�������
	//��ʱ�ļ��͹��涼��Ĭ�ϵ�
};

//�����������Ȧ
struct STR_SUB_CL_CLUB_JOIN_CLUB
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//����ȦID
	TCHAR	szNote[128];								//����˵��
};

//�����������Ȧ ����
struct STR_CMD_LC_CLUB_JOIN_CLUB
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};
//�����������Ȧ �㲥
struct STR_SUB_CL_CLUB_JOIN_CLUB_BDCAST
{
	DWORD	dwClubID;									//����ȦID
	DWORD	dwApplicantNum;								//��������
};

//��ɢ����Ȧ
struct STR_SUB_CL_CLUB_DISS_CLUB
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//����ȦID
	TCHAR	szNote[128];								//��ɢ˵��
};

//��ɢ����Ȧ ����
struct STR_CMD_LC_CLUB_DISS_CLUB
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//������������Ȧ�б�
struct STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST
{
	DWORD	dwUserID;									//���ID
};

//������������Ȧ�б� ����
struct STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST
{
	DWORD	dwClubID;									//����ȦID
	TCHAR	szClubName[LEN_NICKNAME];					//����Ȧ����

	BYTE	byClubRole;									//���ְ�� 0Ⱥ�� 1���� 2һ���Ա
};

//��������Ȧ�б� ���
struct STR_CMD_LC_CLUB_ALL_INFO_FINISH
{
	BYTE	byMask;										//��ɱ�־ 1
	DWORD	dwUserID;									//����ID
};

//����δ��Ա�������������Ȧ(���9��)
struct STR_SUB_CL_CLUB_RANDOM_CLUB_LIST
{
	DWORD	dwUserID;									//���ID
};

//����δ��Ա�������������Ȧ(���9��) ����
struct STR_CMD_LC_CLUB_RANDOM_CLUB_LIST
{
	DWORD	dwClubID;									//����ȦID
	TCHAR	szClubName[LEN_NICKNAME];					//����Ȧ����
	DWORD	dwMajorKindID;								//������ϷID
	DWORD	dwMinorKindID[10];							//��Ҫ��Ϸ ���10��
	BYTE	byClubLevel;								//����Ȧ�ȼ�
	WORD	wMemberNum;									//��ǰ����
	TCHAR	szNotice[128];								//����Ȧ ����
	TCHAR	szMessage[128];								//����Ȧ ���
	DWORD	dwClubOwner;								//Ⱥ��ID
	TCHAR	szClubOwnerName[LEN_NICKNAME];				//Ⱥ������
};

//����δ��Ա�������������Ȧ(���9��) ����
struct STR_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH
{
	BYTE	byMask;										//������־ 1
};
#pragma endregion

#pragma region 2. ����Ȧ����
//��ѯ�ض�����Ȧ�����б�
struct STR_SUB_CL_CLUB_ROOM_LIST
{
	DWORD	dwClubID;									//����ȦID
};

//��ѯ�ض�����Ȧ�����б� ����
struct STR_CMD_LC_CLUB_ROOM_LIST
{
	DWORD	dwRoomID;									//����Ȧ����ID
	TCHAR	szRoomName[16];								//club��������
	DWORD	dwKindID;									//��Ϸ���� -- ��Ϸ����ַ
	BYTE	byGoldOrFK;									//�������� 1������  2��ҳ�
	TCHAR	szKindName[LEN_KIND];						//��Ϸ����
	WORD	wPlayerNum;									//�÷����ڵ�ǰ�����
	TCHAR	szRoomRule[128];							//�������˵��
	BYTE	byAllRound;									//��Ϸ�ܾ���
	BYTE	byChairNum;									//������Ŀ

	//��ҷ� && ������ͨ��
	BYTE	bDissolve;									//�Ƿ������ɢ  0����  1������
	DWORD	dwDissolveTime;								//��ɢʱ��

	//��ҷ�����
	DWORD	dwAmount;									//��Ͷ��
	DWORD	dwOwnerPercentage;							//Ⱥ�����

	//��ע && ������
	BYTE	byMask;										//1 AA֧��;  2��Ӯ��֧��
	DWORD	dwDizhu;									//��ע
	DWORD	dwGold;										//������
};

//��ѯ�ض�����Ȧ�����б� ���
struct STR_CMD_LC_CLUB_ROOM_LIST_FINISH
{
	BYTE	byMask;										//��ɱ�־ 1
};

//������뷿��
struct STR_SUB_CL_CLUB_JOIN_ROOM
{
	DWORD	dwUserID;									//���ID
	DWORD	dwRoomID;									//����ID
};

//������뷿�� ����
struct STR_CMD_LC_CLUB_JOIN_ROOM
{
	DWORD	dwPlayerNum;								//��ǰ��������
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//�÷����tableList
struct STR_CMD_LC_CLUB_TABLE_LIST
{
	DWORD	dwClubRoomID;								//����Ȧ����ID
	DWORD	dwTableID;									//����ID
	DWORD	dwClubID;									//���ֲ�ID
	
	BYTE	byMask;										//0-����Ⱥ��;  1-��Ⱥ�� (���ֶ�������)

	BYTE	byDel;										//mask 0�޲���;  1ɾ��
	DWORD	TableState;									//0���ӿ��� 1�����Ѿ����� 2��������
	DWORD	LockState;									//0������Ҫ����  1���Ӳ���Ҫ����
	DWORD	CurrentRound;								//��ǰ����

	DWORD	dwOwnerID;									//Ⱥ��ID
};

//�÷����userList
struct STR_CMD_LC_CLUB_TABLE_USER_LIST
{
	BYTE	byDel;										//mask 0�޲����� 1ɾ��
	DWORD	dwClubRoomID;								//���ֲ�����ID
	DWORD	dwUserID;									//���ID
	TCHAR	szUserName[LEN_NICKNAME];					//�������
	BYTE	bySex;										//�Ա�
	WORD	wLevel;										//�ȼ�
	TCHAR	szHeadUrl[LEN_URL];							//ͷ���ַ

	BYTE	byClubRole;									//���ְ�� 0 Ⱥ��; 1����; 2һ���Ա
	BYTE	byClubReputation;							//����ڹ����е������ȼ�

	DWORD	dwTableID;									//����ID
	BYTE	byChairID;									//����ID
};


//������������
struct STR_SUB_CL_CLUB_ROOM_SETTING
{
	DWORD	dwUserID;									//���ID
	DWORD	dwRoomID;									//����ID
	BYTE	byGoldOrFK;									//0��ҷ�; 1������

	//��ҷ� && ������ ͨ������
	bool	bDissolve;									//�Ƿ������ɢ
	DWORD	dwDissolveTime;								//��ɢʱ��

	//��ҷ�����
	BYTE	byMask;										//1 AA֧��;  2��Ӯ��֧��

	DWORD	dwAmount;									//��Ͷ��(��Ӯ������������ �ſ�ʼ��ˮ)
	DWORD	dwOwnerPercentage;							//Ⱥ�����

	//��ע && ������
	DWORD	dwDizhu;									//��ע
	DWORD	dwGold;										//������

	TCHAR	szRoomName[16];								//��������
};

//������������ ����
struct STR_CMD_LC_CLUB_ROOM_SETTING
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��

	DWORD	dwRoomID;									//����ID
	BYTE	byGoldOrFK;									//0��ҷ�; 1������

	//��ҷ� && ������ ͨ������
	BYTE	byDissolve;									//�Ƿ������ɢ  0�ǲ����� ����������
	DWORD	dwDissolveTime;								//��ɢʱ�� �������� �ͻ��˲���ʹ�ã�

	//��ҷ�����
	DWORD	dwAmount;									//��Ͷ��
	DWORD	dwOwnerPercentage;							//Ⱥ�����

	//��ע && ������
	BYTE	byMask;										//1 AA֧��;  2��Ӯ��֧��
	DWORD	dwDizhu;									//��ע
	DWORD	dwGold;										//������

	TCHAR	szRoomName[16];								//��������
};

//���󷿼�����
struct STR_SUB_CL_CLUB_ROOM_QUERY_SETTING
{			
	DWORD	dwUserID;									//���ID
	DWORD	dwRoomID;									//����ID
};

//���󷿼����� ����
struct STR_CMD_LC_CLUB_ROOM_QUERY_SETTING
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��

	DWORD	dwRoomID;									//����ID
	BYTE	byGoldOrFK;									//0��ҷ�; 1������

	//��ҷ� && ������ ͨ������
	BYTE	byDissolve;									//�Ƿ������ɢ 0�ǲ����� ����������
	DWORD	dwDissolveTime;								//��ɢʱ������������ �ͻ��˲���ʹ�ã�

	//��ҷ�����
	DWORD	dwAmount;									//��Ͷ��
	DWORD	dwOwnerPercentage;							//Ⱥ�����

	//��ע && ������
	BYTE	byMask;										//1 AA֧��;  2��Ӯ��֧��
	DWORD	dwDizhu;									//��ע
	DWORD	dwGold;										//������

	TCHAR	szRoomName[16];								//��������
};

//����뿪����
struct STR_SUB_CL_CLUB_ROOM_USER_LEAVE
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubRoomID;								//���ֲ�����ID(�뿪֮ǰ���ڵķ���ID)
};

//��ɢ��������
struct STR_SUB_CL_CLUB_ROOM_DISSOLVE
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//���ֲ�ID
	DWORD	dwClubRoomID;								//����ID
};

//��ɢ�������� ����
struct STR_CMD_LC_CLUB_ROOM_DISSOLVE
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//��ɢ��������
struct STR_SUB_CL_CLUB_TABLE_DISSOLVE
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//���ֲ�ID
	DWORD	dwClubRoomID;								//����ID
	DWORD	dwTableID;									//����ID
};

//��ɢ�������� ����
struct STR_CMD_LC_CLUB_TABLE_DISSOLVE
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��

	//�����ֶ� �ͻ������账��, ������Ϊ���ֶζ�����
	BYTE	byMask;										//0-�ɹ�  ����ʧ��
	DWORD	dwTableID;									//����ID
	DWORD	dwGameID;									//��ϷID
};
#pragma endregion

#pragma region 3.�������
//����Ȧ���� -- �ڲ���Ա�ɿ�
struct STR_SUB_CL_CLUB_NOTICE
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//����ȦID
	TCHAR	szNotice[128];								//����Ȧ ����
};

//����Ȧ���� ����
struct STR_CMD_LC_CLUB_NOTICE
{
	LONG	lResultCode;								//0�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//����Ȧ���� �㲥
struct STR_CMD_LC_CLUB_NOTICE_BDCAST
{
	DWORD	dwUserID;									//���ID
	TCHAR	szNotice[128];								//����Ȧ ����
};

//����Ȧ���
struct STR_SUB_CL_CLUB_MESSAGE
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//���ID
	TCHAR	szMessage[128];								//����Ȧ ���
};

//����Ȧ��� ����
struct STR_CMD_LC_CLUB_MESSAGE
{
	LONG	lResultCode;								//0�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//���׷���
struct STR_SUB_CL_CLUB_CONTRIBUTE_FK
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//���ֲ�ID
	DWORD	dwFK;										//���׵ķ�������
};

//���׷��� ����
struct STR_CMD_LC_CLUB_CONTRIBUTE_FK
{
	LONG	lResultCode;								//0�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��

	DWORD	dwClubFK;									//��ǰ����Ȧ�з�������
};

//����Ȧ����
struct STR_SUB_CL_CLUB_AUTO_AGREE
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//���ֲ�ID
	BYTE	byAutoAgree;									//�Ƿ����Զ�ͬ�� 0ͬ��
	//����Ϊ�������� ����10�����ϵ�Ů�Բ��������ȵ� -- �������
	BYTE	bySex;										//0������; ����1��; 2Ů
	WORD	wLevel;										//����ȼ�
};

//����Ȧ���� ����
struct STR_CMD_LC_CLUB_AUTO_AGREE
{
	LONG	lResultCode;								//0�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//����Ȧ�ö�
struct STR_SUB_CL_CLUB_STICKY_POST
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//���ֲ�ID
	BYTE	cbTopFlag;									//�Ƿ��ö�	1-�ö�	0-ȡ���ö�
};

//����Ȧ�ö�����
struct STR_CMD_LC_CLUB_STICKY_POST
{
	LONG	lResultCode;								//0�ɹ�  ����ʧ��
};


#pragma endregion

#pragma region 4. ��Ա����
//�����Ա����
struct STR_SUB_CL_CLUB_MEMBER_MANAGER
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//����ȦID
};

//�����Ա���� ����
struct STR_CMD_LC_CLUB_MEMBER_MANAGER
{
	DWORD	dwUserID;									//���ID
	TCHAR	szUserName[LEN_NICKNAME];					//�������
	BYTE	byClubRole;									//���ְ�� 0 Ⱥ��; 1����; 2һ���Ա
	BYTE	byUserLevel;								//��ҵȼ�
	BYTE	byClubReputation;							//����ڹ����е������ȼ�

	BYTE	byUserState;								//���״̬  0���� 1���� 2��Ϸ��
	TCHAR	szHeadUrl[LEN_URL];							//ͷ���ַ
};

//�����Ա���� ���
struct STR_CMD_LC_CLUB_MEMBER_MANAGER_FINISH
{
	BYTE	byMask;										//��ɱ�־ 1
};

 //ͬʱ���صĹ��������Ϣ
struct STR_CMD_LC_CLUB_DATA
{
	TCHAR	szClubName[LEN_NICKNAME];					//Ⱥ����
	TCHAR	szClubOwnerName[LEN_NICKNAME];				//Ⱥ������
	DWORD	dwClubID;									//ȺID
	DWORD	dwClubOwner;								//Ⱥ��ID
	DWORD	dwMajorKindID;								//������ϷID 
	DWORD	dwMinorKindID[10];							//��Ҫ��Ϸ ���10��
	byte	byClubLevel;								//����Ȧ�ȼ�
	WORD	wMemberNum;									//��ǰ����
	TCHAR	szNotice[128];								//����Ȧ����
	TCHAR	szMessage[128];								//����Ȧ���
	DWORD	dwFK;										//����Ȧ�з�������
	BYTE	byApplyNum;									//����Ȧ�з�������
	BYTE	byAutoAgree;								//�µ���Ⱥ������
	BYTE	bySex;										//�Ƿ����Զ�ͬ��
	WORD	wLevel;										//����ȼ�
};

//�������˼�������Ȧ
struct STR_SUB_CL_CLUB_INVITE
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//����ȦID
	DWORD	dwTagID;									//Ŀ�����ID
};

//�������˼�������Ȧ ����
struct STR_CMD_LC_CLUB_INVITE
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//�������˵�����
struct STR_CMD_LC_CLUB_INVITE_REMIND
{
	DWORD	dwTagID;									//Ŀ��ID
};

//�������˵Ļظ�
struct STR_SUB_CL_CLUB_INVITE_RESULT
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//����ȦID
	byte	byAgree;									//0ͬ��; 1��ͬ��
};

//�������˵Ļظ� ����
struct STR_CMD_LC_CLUB_INVITE_RESULT
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//�������б�
struct STR_SUB_CL_CLUB_APPLICANT_LIST
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;									//����ȦID
};

// �������б� ����
struct STR_CMD_LC_CLUB_APPLICANT_LIST
{
	DWORD	dwUserID;									//�������ID
	TCHAR	szUserName[LEN_NICKNAME];					//��������ǳ�
	BYTE	byUserSex;									//����Ա�
	BYTE	byUserLevel;									//��ҵȼ�
	TCHAR	szNote[128];								//����˵��

	TCHAR	szHeadUrl[LEN_URL];							//ͷ���ַ
};

//�������б� ����
struct STR_CMD_LC_CLUB_APPLICANT_LIST_FINISH
{
	BYTE	byMask;										//������־ 1
};

//�������˲鿴�Լ��������б�
struct STR_SUB_CL_CLUB_INQUERY_LIST
{
	DWORD	dwUserID;									//���ID
};

//�������˲鿴�Լ��������б� ����
struct STR_CMD_LC_CLUB_INQUERY_LIST
{
	DWORD	dwClubID;									//����ȦID
	TCHAR	szClubName[LEN_NICKNAME];					//��������ǳ�
	DWORD	dwMajorKindID;								//������ϷID
	DWORD	dwMinorKindID[10];							//�δ���ϷID
	BYTE	byClubLevel;								//����Ȧ�ȼ�
	WORD	wMemberNum;									//��ǰ����
	TCHAR	szNotice[128];								//����Ȧ ����
	TCHAR	szMessage[128];								//����Ȧ ���
	DWORD	dwClubOwner;								//Ⱥ��ID
	TCHAR	szClubOwnerName[LEN_NICKNAME];				//Ⱥ���ǳ�
};

//�������˲鿴�Լ��������б� ����
struct STR_CMD_LC_CLUB_INQUERY_LIST_FINISH
{
	BYTE	byMask;										//������־ 1
};

//Ⱥ��|���� 
struct STR_SUB_CL_CLUB_APPLICANT_RESULT
{
	DWORD	dwUserID;									//���ID
	DWORD	dwCludID;									//���ֲ�ID
	DWORD	dwResult;									//0ͬ�� �����ܾ�
	DWORD	dwTagID;									//������ID
};

//Ⱥ��|����  ����
struct STR_CMD_LC_CLUB_APPLICANT_RESULT
{
	LONG	lResultCode;								//0-�ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//�˳�����Ȧ 
struct STR_SUB_CL_CLUB_QUIT
{
	DWORD	dwClubID;									//����ȦID
	DWORD	dwUserID;									//���ID
};

//�˳�����Ȧ ����
struct STR_CMD_LC_CLUB_QUIT
{
	LONG	lResultCode;								//0-����ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//ְ������ ����
struct STR_SUB_CL_CLUB_APPOINTMENT
{
	DWORD	dwClubID;									//����ȦID
	DWORD	dwUserID;									//���ID
	DWORD	dwTagID;									//Ŀ�����ID
	BYTE	byAction;									//0Ϊ�߳��û�; 1��Ϊƽ��; 2��Ϊ����Ա
};

//ְ������ ����
struct STR_CMD_LC_CLUB_APPOINTMENT
{
	LONG	lResultCode;								//������
	TCHAR	szDescribe[128];							//������Ϣ
};

//ְ������ ����
struct STR_CMD_LC_CLUB_APPOINTMENT_NOTE
{
	DWORD	dwUserID;									//Ŀ�����ID
	BYTE	byAction;									//0�߳�  1��Ϊ��ͨ��Ա 2����Ա
	TCHAR	szDescribe[128];							//������Ϣ
};

//����Ȧ����
struct STR_SUB_CL_CLUB_CHAT
{
	DWORD	dwUserID;									//���ID
	DWORD	dwTagUserID;								//˽��ʱ���͵���ҵ�ID
	BYTE	byChatMode;									//0.���ֲ�����  1.��������   2.ϵͳ����    3.˽��
	DWORD	dwClubID;									//���ֲ�ID
	TCHAR	szChat[128];								//��������
};
//����Ȧ���� ����
struct STR_CMD_LC_CLUB_CHAT
{
	BYTE	byChatMode;									//0.���ֲ�����  1.��������   2.ϵͳ����    3.˽��
	LONG	lResultCode;								//0-����ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};
//����Ȧ���� �㲥
struct STR_CMD_LC_CLUB_CHAT_BDCAST
{
	BYTE	byChatMode;									//0.���ֲ�����  1.��������   2.ϵͳ����    3.˽��

	DWORD	dwClubID;									//���ֲ�ID
	DWORD	dwUserID;									//�������ID
	DWORD	dwTagID;									//Ŀ�����ID

	TCHAR	szUserNickName[32];							//��������ǳ�
	TCHAR	szHeadUrl[256];								//�������ͷ���ַ
	TCHAR	szChat[128];								//���������������
};

//ʵʱˢ�¹����б�
struct STR_CMD_LC_CLUB_LIST_RE
{
	BYTE	byMask;					//��ʵ������
};

//ʵʱˢ�¹��᷿���б�
struct STR_CMD_LC_CLUB_ROOM_RE
{
	DWORD	dwClubID;				//���ֲ�ID
};

#pragma endregion

#pragma region 5. ս���鿴
//����ս��ͳ�� ��ѯ
struct STR_SUB_CL_CLUB_RECORD_LIST
{
	DWORD						dwUserID;					//���ID
	DWORD						dwClubID;					//����Ȧ(�汾2)����, Ϊ0��ʾ�Ǵ���
	SYSTEMTIME					tmQueryStartData;			//��Ҫ��ѯ�Ŀ�ʼ����(��С��λΪ��)
	SYSTEMTIME					tmQueryEndData;				//��Ҫ��ѯ�Ľ�ֹ����(��С��λΪ��)
};
//����ս��ͳ�� ����
struct STR_CMD_LC_CLUB_RECORD_LIST
{
	BYTE						byMask;						//1������  2��ҳ�

	DWORD						dwUserID;					//���ID
	TCHAR						szNickName[32];				//�ǳ�
	TCHAR						szHeadUrl[LEN_HEAD_URL];	//ͷ��

	DWORD						dwAllCount;					//�ܳ���
	SCORE						lAllScore;					//����ܳ�����

	DWORD						dwWinCount;					//��Ӯ�ҳ���
	SCORE						lWinScore;					//��Ҵ�Ӯ�ҷ���
};

//����ս��ͳ�� ����
struct STR_CMD_LC_CLUB_RECORD_FINISH
{
	BYTE						byMask;						//������־
};

#pragma endregion
#pragma region ��ɾ��
//��������Ȧ����
struct STR_SUB_CL_CLUB_CREATE_CLUB_ROOM_APPLY
{
	DWORD	dwUserID;									//���ID
	DWORD	dwClubID;
	DWORD	dwKindID;									//��Ϸ��������
};

//��������Ȧ���� ����
struct STR_CMD_LC_CLUB_CREATE_CLUB_ROOM_APPLY
{
	LONG	lResultCode;								//0-����ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
};

//����Ȧ�������� ����
struct STR_CMD_LC_CLUB_CREATE_ROOM_INFO
{
	LONG	lResultCode;								//0-����ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
	DWORD	dwRoomID;									//����ID
};

//�����������
struct STR_SUB_CL_CLUB_JOIN_TABLE
{
	DWORD	dwUserID;									//���ID
	int		JoinType;									//��������
	DWORD	dwTableID;									//����ID
	DWORD	dwChairID;									//����ID
	DWORD	dwCreatePassword;							//�������뷿����
	DWORD	dwJoinPassword;								//�������뷿����
};

//����������� ����
struct STR_CMD_LC_CLUB_JOIN_TABLE
{
	LONG	lResultCode;								//0-����ɹ�  ����ʧ��
	TCHAR	szDescribe[128];							//ʧ��ԭ��
	DWORD	dwTableID;									//����ID
	DWORD	dwChairID;									//����ID
};

//����״̬
struct STR_CMD_LC_ROOM_STATE
{
	DWORD	dwClubID;									//����Ȧ���
	DWORD	dwStateType;
	DWORD	dwJoinOrLeave;
	DWORD	dwTableState;
	DWORD	dwRoomID;
	DWORD	dwUserID;
	DWORD	dwTableID;
	DWORD	dwChairID;

	DWORD	dwCustomID;
	TCHAR	szHeadUrl[LEN_URL];
	TCHAR	szUserName[LEN_NICKNAME];
	TCHAR	szKindName[LEN_KIND];

	DWORD	byPlayerNum;

	BYTE	rule[128];
};
#pragma endregion

#pragma endregion

#pragma region MDM_SHOP �̳ǵ���
//��ѯ�̳���Ʒ�������
struct STR_SUB_CL_SHOP_QUERY
{
	DWORD dwUserID;								//���ID
	BYTE byGoodsType;							//�̳���Ʒ���� 1���� 2��� 3������� 4��������
};

//��ѯ�̳Ƿ���
struct STR_CMD_LC_SHOP_QUERY_RESULT
{
	DWORD dwGoodsID;							//��Ʒ���
	BYTE  byDiscount;							//�ۿ�
	DWORD dbSpreadScore;						//�ƹ����
	DWORD dwPrice;								//�۸�, rmb��������rmb��ֵ�� ��ҹ������ǽ��ֵ�� ���ֹ������ǻ���ֵ
};

//��ѯ�̳ǽ���
struct STR_CMD_LC_SHOP_QUERY_FINISH
{
	BYTE byMask;								//������־ 1			
};

//��ʯ�������
struct STR_SUB_CL_SHOP_DIAMOND
{
	DWORD dwShopper;							//������
	DWORD byGoodsType;							//��Ʒ����
	DWORD dwGoodsID;							//��Ʒ���
	DWORD dwGoodsNum;							//������Ʒ������

	BYTE  byMask;								//0Ϊ�Լ�����;  1Ϊ���˴���
	DWORD dwTargetID;							//���Ͷ���
};

//��ʯ������� ����
struct STR_CMD_LC_SHOP_DIAMOND_RESULT
{
	LONG	lResultCode;						//0-����ɹ�  ����ʧ��
	TCHAR	szDescribe[128];					//ʧ��ԭ��
};

//������Ʒ��ѯ
struct STR_SUB_CL_BAG_QUERY
{
	DWORD dwUserID;				//���ID
};

//������Ʒ��ѯ ����
struct STR_CMD_LC_BAG_RESULT
{
	DWORD dwGoodsID;		//��ƷID
	DWORD dwGoodsNum;		//��Ʒ����
	BYTE  byGoodsType;		//��Ʒ����
};

//������Ʒ��ѯ ����
struct STR_CMD_LC_BAG_FINISH
{
	DWORD dwLoveness;		//�������ֵ
};

#pragma endregion

#pragma region MDM_GIFT �������
//���͵���
struct STR_SUB_CL_GIFT_GIVE_PROPS  
{
	DWORD										dwSrcUserID;				//�۳��û�ID
	DWORD										dwDstUserID;				//Ŀ���û�ID
	DWORD										dwPropsID;					//���߱��
	DWORD										dwCount;					//��������
};

//���͵��� ����
struct STR_CMD_LC_GIFT_GIVE_PROPS  
{
	DWORD										dwResultCode;				//�����ʶ
	TCHAR										szDescribeString[128];		//������Ϣ

	DWORD										dwPropsID;					//���߱��
	DWORD										dwCount;					//��������

	DWORD										dwLoveness;					//����ֵ
};

//�������� -- �������˵�����
struct STR_CMD_LC_GIFT_GIVE_PROPS_SHOW
{
	DWORD							dwTargetID;							//���ID
	TCHAR							szDescribeString[128];				//������Ϣ
};
#pragma endregion

#pragma region MDM_GAME ��Ϸ�����
//��������֮ǰ, �Ȳ�ѯ���õ�GameID
struct STR_SUB_CL_GAME_QUERY_GAMEID
{
	DWORD	dwKindID;		//TODONOW ȷ��kindID������
};

//��������֮ǰ, �Ȳ�ѯ���õ�GameID
struct STR_CMD_LC_GAME_QUERY_GAMEID
{
	//�������
	DWORD                           dwResultCode;							//�����ʶ
	TCHAR							szDescribeString[LEN_MESSAGE_DESCRIBE];	//������Ϣ

	DWORD	dwGameID;				//���õ�GameID;
};
#pragma endregion
#pragma pack()

#endif