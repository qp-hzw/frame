#ifndef CMD_LONGON_HEAD_FILE
#define CMD_LONGON_HEAD_FILE

#include "CMD_Club.h"
#include "STR_CMD_LogonServer.h"

/*
** ��Ҫ����:  ��¼�� �� �ͻ��˵� ��Ϣ
** ��ϸ����:  1. ��Ϣ��(�� + ��)     2. �ṹ��
** ��ע˵��:  ����Ϣ�ŵĺ���: 
**            1. ��һ���ֶ�: ������־,        
**				 SUB(client->server����Ϣ)�� CMD(server->client����Ϣ), 
**				 DBR(�����ݿ�Request)��DBO(���ݿⷵ�ص���Ϣ).
**            2. �ڶ����ֶ�: ��Ϣ����         
**				 C:client;
**            3. �������ֶ�: ����Ϣ�ŵı�־   ����LOGON��Ϊ��¼ģ�����Ϣ
**            4. ֮����ֶ�: ����Ϣ�ŵĺ���   ����Accounts��ʾ�˺ŵ�¼
**           
**            �����淶
**            1. �ṹ�������:  ����Ϣ�ŵ�ǰ���STR, ����SUB_CL_LOGON_ACCOUNTS ��Ϊ STR_SUB_CL_LOGON_ACCOUNTS
**
**            2. DBO, DBR��Ϣ�ŵ�����Ϊ:  ��SUB�滻ΪDBR��DBO����
**
**            3. �����������淶: 1) ָ���p  2)������־SUB  3)����Ϣ�ű�־  4)����Ϣ�ű�־
**
**            4. ����������:  On_������־_����Ϣ������Ϣ��
**
**
*/

#pragma region MDM_LOGON ��¼ģ��
/* *********************************************************************************
**      MAIN:1   MDM_LOGON    ��¼ ģ��
** *********************************************************************************/
#define MDM_LOGON					1									//����Ϣ

#define CMD_LC_COMMON_ERROR		    0									//ͨ�ô��� 

#define SUB_CL_LOGON_ACCOUNTS		1									//�ʺŵ�¼
#define SUB_CL_LOGON_REGISTER	    2									//�ʺ�ע�ᣨ���ص��ǵ�¼�ɹ���
#define SUB_CL_LOGON_PLATFORM		3									//ƽ̨��½����������¼��
#define SUB_CL_LOGON_TEMP		    4									//�ο͵�½

#define CMD_LC_LOGON_ACCOUNTS		101									//��¼�ɹ�
#define CMD_LC_LOGON_PLATFORM       103                                 //ƽ̨��½����
#define CMD_LC_LOGON_REPEAT_LOGON	104									//�ظ���¼
#define CMD_LC_LOGON_LOGON_REWARD   105                                 //��¼����������һع� + ���ս�����


#pragma endregion

#pragma region MDM_SERVICE �û�����
/* *********************************************************************************
**      MAIN:3    MDM_SERVICE    ����		//�����������пͻ��˴��������ť��Ϣ
**		˵����1��C(�ͻ���)->L(��½��)����Ϣ�ŷ�Χ��	1-100
**			  2��L(�ͻ���)->C(��½��)����Ϣ�ŷ�Χ��	101-200
**			  3��������Ϣ�ŷ�Χ��	201-300���������������ͣ�
** *********************************************************************************/
//������Ϣ�š�
#define MDM_SERVICE								3							//����

//������Ϣ�š�
#define SUB_CL_SERVICE_FLOWER					1							//��˿, ��ע
#define CMD_LC_SERVICE_FLOWER					101							//��˿, ��ע ����

#define SUB_CL_SERVICE_FLOWER_QUERY				2							//�鿴��ע
#define CMD_LC_SERVICE_FLOWER_QUERY				102							//�鿴��ע ����
#define CMD_LC_SERVICE_FLOWER_QUERY_FINISH		202							//�鿴��ע ����

#define SUB_CL_SERVICE_FLOWING_QUERY			3							//�鿴��˿
#define CMD_LC_SERVICE_FLOWING_QUERY			103							//�鿴��˿ ����
#define CMD_LC_SERVICE_FLOWING_QUERY_FINISH		203							//�鿴��ע ����

#define SUB_CL_SERVICE_FLOWER_ACT			    4							//��ע, ȡ����ע
#define CMD_LC_SERVICE_FLOWER_ACT			    104							//��ע, ȡ����ע ����

#define SUB_CL_SERVICE_REFRESH_USER_INFO		5							//ˢ���û���Ϣ
#define CMD_CL_SERVICE_REFRESH_USER_INFO		105							//ˢ���û���Ϣ����

#define SUB_CL_SERVICE_MODIFY_PERSONAL_INFO		12							//�޸ĸ������ϣ������û����޸ĵ����϶������ﴦ��
#define CMD_LC_SERVICE_MODIFY_PERSONAL_INFO		112							//�޸ĸ������Ϸ���


/* START �ϴ�����ս��, ֮�����ɾ���� */
#define SUB_CL_SERVICE_GET_DJ_RECORD_LIST		6							//��ȡ���¼���б�
#define CMD_LC_SERVICE_GET_DJ_RECORD_LIST		106							//��ȡ���¼���б���

#define SUB_CL_SERVICE_GET_XJ_RECORD_LIST		7							//��ȡС��¼���б�
#define CMD_LC_SERVICE_GET_XJ_RECORD_LIST		107							//��ȡС��¼���б���
/* END */

#define SUB_CL_SERVICE_ONLINE_REWARD			8							//��ȡ���߽��������ս���������ȡ������
#define CMD_LC_SERVICE_ONLINE_REWARD			108							//��ȡ���߽�������

#define SUB_CL_SERVICE_GET_TASK_LIST			9							//��ȡ�����б�			//TODO ����״̬�ĸ����߼���û�޸�
#define CMD_LC_SERVICE_GET_TASK_LIST			109							//��ȡ�����б���

#define SUB_CL_SERVICE_GET_TASK_REWARD			10							//��ȡ������
#define CMD_LC_SERVICE_GET_TASK_REWARD			110							//��ȡ����������

#define SUB_CL_SERVICE_REQUEST_LOTTERY			11							//����齱
#define CMD_LC_SERVICE_REQUEST_LOTTERY			111							//����齱����

#define CMD_LC_USER_QUERY_SCORE_INFO			114							//��ѯ���޸ģ��û���ҷ�����ʯ��Ϣ����(�漰����ҷ����ı�Ķ����õ������Ϣ)

#define SUB_CL_SERVICE_PURE_STANDING_LIST		15							//pure�������а� ��ѯ
#define CMD_LC_SERVICE_PURE_STANDING_LIST		115							//pure�������а� ����
#define CMD_LC_SERVICE_PURE_STANDING_FINISH		215							//pure�������а� ����

#define SUB_CL_SERVICE_PURE_RECORD_LIST			16							//���ս�� ��ѯ
#define CMD_LC_SERVICE_PURE_RECORD_LIST			116							//���ս�� ����

#define SUB_CL_SERVICE_PURE_XJ_RECORD_LIST		17							//С��ս�� ��ѯ
#define CMD_LC_SERVICE_PURE_XJ_RECORD_LIST		117							//С��ս�� ����

#define CMD_LC_SERVICE_MARQUEE					18							//�������Ϣ

#define SUB_CL_SERVICE_XJ_RECORD_PLAYBACK		20							//С��¼��ط�
#define CMD_LC_SERVICE_XJ_RECORD_PLAYBACK		120							//С��¼��ط� ����
#define CMD_LC_SERVICE_XJ_RECORD_PLAYERINFO		220							//С��¼��ط� �����Ϣ

#define SUB_CL_SERVICE_GET_RANK_LIST			21							//��ȡָ�����а�			TOOD �����������Ǵ�����Ϸ��ȡ�ģ�С�ֽ��㣬��ֻ�н��յ����ݣ�û�и��µ�����֮���
#define CMD_LC_SERVICE_GET_RANK_LIST			121							//��ȡָ�����а񷵻�

#define SUB_CL_SERVICE_GET_RANK_REWARD			22							//��ȡ���а���
#define CMD_LC_SERVICE_GET_RANK_REWARD			122							//��ȡ���а�������

#pragma endregion

#pragma region MDM_SHOP �̳ǵ���
/* *******************************************************************************
**          MAIN:7      MDM_SHOP   �̳ǵ���
** ******************************************************************************/
//������Ϣ�š�
#define MDM_SHOP								7							//�̳ǵ���

//������Ϣ�š�
#define SUB_CL_SHOP_BUY							1							//�̳ǹ���
#define CMD_LC_SHOP_BUY_RESULT					101							//�̳ǹ��򷵻�

#define SUB_CL_BAG_QUERY						2							//������Ʒ��ѯ
#define CMD_LC_BAG_RESULT						102							//������Ʒ����
#define CMD_LC_BAG_FINISH						202							//������Ʒ��ѯ����

#define SUB_CL_GIFT_GIVE_PROPS					3							//���͵���
#define CMD_LC_GIFT_GIVE_PROPS					103							//���͵��߷���
#define CMD_LC_GIFT_GIVE_PROPS_SHOW				203							//�������˵�����

#define SUB_CL_OTHERS_RECHARGE_INFO				4							//��ֵ��Ϣ
#define CMD_LC_OTHERS_RECHARGE_INFO				104							//��ֵ��Ϣ ����

#define SUB_CL_OTHERS_EXCHANGE_INFO				5							//�һ���Ϣ��Ǯ->���ߣ�
#define CMD_LC_OTHERS_EXCHANGE_INFO				105							//�һ���Ϣ����


//��ֵ��Ϣ
struct STR_SUB_CL_OTHER_RECHARGE_INFO
{
	DWORD								dwUserID;							//�û� I D
};
//��ֵ��Ϣ����
struct STR_CMD_LC_OTHER_RECHARGE_INFO
{
	SCORE								dwMinMoney;		//���ٳ�ֵǮ  
	UINT32								dwChangeScale;  //�һ�����
};

//�һ�����
struct STR_SUB_CL_OTHER_EXCHANGE_INFO
{
	DWORD								dwUserID;							//�û� I D
};
//�һ����߷���
struct STR_CMD_LC_OTHER_EXCHANGE_INFO
{
	SCORE								dwMinMoney;		///���ٳ�ֵǮ  
	UINT32								dwChangeScale;  ///�һ�����
	SCORE								dwWithdrawals;  ///������RMB
	SCORE								dwBankMoney;	///���н��
};

//���ò������
struct STR_CMD_LC_OTHER_OPERATE_RESULT
{
	LONG							lResultCode;						//�������
};
#pragma endregion

#pragma region MDM_GAME ��Ϸ�����
#define MDM_GAME								8							//��Ϸ�����

//client->logon, logon->cpd, cpd->logon, logon->client
#define SUB_CL_GAME_QUERY_GAMEID				1							//��������֮ǰ, �Ȳ�ѯ���õ�GameID
#define CMD_LC_GAME_QUERY_GAMEID				101							//��������֮ǰ, �Ȳ�ѯ���õ�GameID

#pragma endregion

#endif