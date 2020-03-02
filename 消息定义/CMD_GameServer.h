#ifndef CMD_GAME_SERVER_HEAD_FILE
#define CMD_GAME_SERVER_HEAD_FILE

#include "STR_CMD_GameServer.h"
#include "CMD_Club.h"

/*
** ��Ҫ����:  ��Ϸ�� �� �������̵� ��Ϣ
** ��ϸ����:  1. ��Ϣ��(�� + ��)     2. �ṹ��
** ��ע˵��:  ����Ϣ�ŵĺ���: 
**            1. ��һ���ֶ�: ������־,        SUB(���ܵ�����Ϣ)�� CMD(���ͳ�ȥ����Ϣ), DBR(�����ݿ�Request)��DBO(���ݿⷵ�ص���Ϣ)
**            2. �ڶ����ֶ�: ��Ϣ����         L: logon;  G:gameserver;  C:client; CP:correspond;  W:web  S:���������Ϸ R:�ͻ�������Ϸ
**            3. �������ֶ�: ����Ϣ�ŵı�־   ����LOGON��Ϊ��¼ģ�����Ϣ
**            4. ֮����ֶ�: ����Ϣ�ŵĺ���   ����Accounts��ʾ�˺ŵ�¼
**           
**            �����淶
**            1. �ṹ�������:  ����Ϣ�ŵ�ǰ���STR, ����SUB_CG_LOGON_ACCOUNTS ��Ϊ STR_SUB_CG_LOGON_ACCOUNTS
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
**						MAIN:1   MDM_LOGON    ��¼ģ��
** *********************************************************************************/
#define MDM_GR_LOGON				1										//��¼��Ϣ

#define CMD_GC_COMMON_ERROR		    0									    //ͨ�ô��� 
//��¼
#define SUB_CG_LOGON_USERID			1										//ID ��¼
#define CMD_GC_LOGON_USERID			101										//ID ��¼����

#pragma endregion

#pragma region MDM_USER �û�����
/* *********************************************************************************
**							MAIN:2   MDM_USER    �û�����
** *********************************************************************************/
#define MDM_USER						2									//�û���Ϣ

//
#define CMD_GC_USER_MODIFY_TREASURE		1									//�޸��û��Ƹ���Ϣ����
#define SUB_GR_REQUEST_FAILURE			4									//����ʧ��		�󲿷�ʧ����Ϣ�����������Ϣ


//��Ҷ���	
#define SUB_CG_USER_SITDOWN				10									//*���������û�����/��������ɹ��󣬷�����������������£�����Ҫ����
#define SUB_CG_USER_READY				11									//�û�׼��
#define SUB_CG_USER_STANDUP				12									//#��������		TODO �Ƿ�����ɢ���䣬���͵��Ǹ���Ϣ������15�Ĵ���������
#define SUB_GR_USER_CHAIR_REQ			13                                  //*�������λ��

#define SUB_CG_USER_INVITE_USER			14									//*�����û���������
#define CMD_GC_USER_INVITE_USER			15									//*�����û��������ӷ���
#define SUB_CG_USER_KICK_USER			16                                   //*�߳��û�		TODO ���������������ϵͳ��Ϣ���߳��û�

#pragma region ����| ���� ����
//������, ������ҳ�
#define SUB_CG_USER_CREATE_ROOM			17									//���봴������
#define CMD_GC_USER_GET_ROOM_RULE		117									//�������ѡ��, ���봴������ɹ�����
#define SUB_CG_USER_SET_ROOM_RULE		217									//����������� ����
#define CMD_GC_USER_ENTER_SUBGAME_ROOM	317									//��������Ϸ

#define SUB_CG_USER_JOIN_FK_ROOM		18									//�������� ��Ҫ����

//��ҳ�
#define SUB_CG_USER_JOIN_GOLD_HALL_ROOM	20									//�����Ҵ��� ��ҳ�����
#define SUB_CG_USER_CHANGE_GOLD_TABLE	21									//��ҳ�����
#pragma endregion

#define SUB_GR_GET_TABLELIST			22									//��ȡ�����б�		TODO ָ���Ǿ��������ѿ������б�
#define SUB_GR_GET_TABLELIST_RESULT		122									//�����б���		

//��ɢ����
#define SUB_RG_USER_ASK_DISMISS			24									//���������ɢ����
#define CMD_GR_USER_ASK_DISMISS_RESULT	124									//���������ɢ���� ���
#define	SUB_RG_USER_VOTE_DISMISS		224									//�����ɢ����
#define CMD_GR_USER_VOTE_DISMISS		324									//�㲥���������ɢ
#define	CMD_GR_USER_DISMISS_RESULT		424									//�㲥�����ɢ������

#define SUB_CG_USER_GOLD_INFO		    25									//�����Ҵ�����Ϣ
#define CMD_GC_USER_GOLD_INFO		    125									//�����Ҵ�����Ϣ ����
#define CMD_GC_USER_GOLD_INFO_FINISH	225									//�����Ҵ�����Ϣ ����

#pragma endregion

#pragma region  MDM_GR_MATCH ��������
/* *********************************************************************************
**						MAIN:7   MDM_GR_MATCH    ��������
** *********************************************************************************/
#define MDM_GR_MATCH				7									//��������

#define SUB_CG_MATCH_INFO			1									//�����������Ϣ
#define CMD_GC_MATCH_INFO			101									//�����������Ϣ  ����

#define SUB_CG_MATCH_APPLY			2									//����������
#define CMD_GC_MATCH_APPLY			102									//����������	����
#define SUB_CG_MATCH_UNAPPLY		3									//���ȡ������
#define CMD_GC_MATCH_UNAPPLY		103									//���ȡ������  ����

#define CMD_GC_MATCH_START			4									//������ʼ
#define CMD_GC_MATCH_CANCEL			5									//����ȡ��

#define SUB_CG_MATCH_QUERY_PLAYER   6									//������Ϣ ����ʱ��
#define CMD_GC_MATCH_QUERY_PLAYER   106									//������Ϣ ����ʱ��  ����

#define SUB_CG_MATCH_RANKING		7									//��������  ������
#define CMD_GC_MATCH_RANKING		107									//��������  ������  ����

#define SUB_CG_MATCH_RANKING_MY		8									//��������  �Լ�
#define CMD_GC_MATCH_RANKING_MY		108									//��������  �Լ�  ����

#define CMD_GC_MATCH_RESULT_JINJI	9									//�����׶ν�� ��ҽ���
#define CMD_GC_MATCH_RESULT_TAOTAI	10									//�����׶ν�� �����̭

#define CMD_GC_MATCH_JUESAI_RECODE	11									//����
#define CMD_GC_MATCH_WAIT_COUNT		12									//�ȴ�������Ϣ

#pragma endregion

#pragma region MDM_G_FRAME �������
/* *********************************************************************************
**							MAIN:100   MDM_G_FRAME    �������
** *********************************************************************************/
#define MDM_G_FRAME					100									//�������

//������Ϣ
#define SUB_RG_ROOM_RULE					1								//���󷿼����
#define CMD_GR_ROOM_RULE					101								//�������
#define SUB_RG_PLAYER_INFO					201								//�����û���Ϣ
#define CMD_GR_USER_STATUS					301								//�û��������
#define CMD_GR_USER_STATUS_FINISH			401								//�����û���Ϣ���
#define SUB_RG_FRAME_GAME_OPTION			501								//������Ϸ����
#define CMD_GR_FRAME_GAME_OPTION			601								//�����û���������

#define CMD_GR_FRAME_GAME_STATUS			102								//��Ϸ״̬
#define CMD_GR_FRAME_GAME_DISSMISS			701							    //��ɢ���״̬

//GPS���
#define SUB_CG_COM_CHECK_USER_GPS			5								//����У���û�GPSλ����Ϣ
#define CMD_GC_COM_CHECK_USER_GPS			105								//У���û�GPSλ����Ϣ����

//IPУ��
#define SUB_CG_COM_CHECK_USER_IP			8								//����У���û�IP��Ϣ
#define CMD_GC_COM_CHECK_USER_IP			108								//У���û�IP��Ϣ����

//�û�����
#define SUB_RG_FRAME_CHAT					7								//�û�����
#define CMD_GR_FRAME_CHAT					107								//�û�����


//��������
#define SUB_CG_EFFECT						9								//�������� -- ����Ϸ���Ӽ���, �ʻ�, ը��
#define CMD_GC_EFFECT_RESULT				109								//����ʧ��
#define CMD_GC_EFFECT_BRODCAST				209								//�����㲥

//����clientû��
#define SUB_GF_LOOKON_CONFIG			803									//�Թ�����

#define SUB_GF_XUFEI					805									//*������Ϣ

#define SUB_GF_USER_EXPRESSION			811									//*�û�����
#define SUB_GF_SOUND					812									//*��������

#define CMD_GR_FRAME_TREASURE_NO		105									//*�û��Ƹ������Լ����¾���Ϸʱ,���͸�client 


//��Ϸ��Ϣ
#define CMD_GF_LOOKON_STATUS			803									//*�Թ�״̬

//ϵͳ��Ϣ
#define CMD_GF_SYSTEM_MESSAGE			806									//ϵͳ��Ϣ

#pragma endregion

#pragma region MDM_GF_GAME ����Ϸ����
/* *********************************************************************************
**							MAIN:200   MDM_GF_GAME    ����Ϸ����
** *********************************************************************************/
#define MDM_GF_GAME					200									//��Ϸ����

#pragma endregion

#endif