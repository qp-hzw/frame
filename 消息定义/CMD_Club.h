#ifndef CMD_CLUB_HEAD_FILE
#define CMD_CLUB_HEAD_FILE

#include "STR_CMD_Club.h"

/*
** ����ϵͳ
** 1. ��¼��, ��Ϸ����Ϣ�Ŷ�������
** 2. L->logon;  G->Game
*/

/* *******************************************************************************
**          MAIN:6      MDM_CLUB  ����
** ******************************************************************************/
//������Ϣ�š�
#define MDM_CLUB								6							//����

//������Ϣ�š�
#define CMD_LC_CLUB_LIST_RE						50							//ʵʱˢ�¹����б�
#define CMD_LC_CLUB_ROOM_RE						51							//ʵʱˢ�¹��᷿���б�

#pragma region ���� ����|����|�б�
#define SUB_CL_CLUB_CREATE_CLUB					1							//������������
#define CMD_LC_CLUB_CREATE_CLUB					101							//������������ ����

#define SUB_CL_CLUB_JOIN_CLUB					2							//������빤��
#define CMD_LC_CLUB_JOIN_CLUB					102							//������빤�᷵��
#define CMD_LC_CLUB_JOIN_CLUB_BDCAST			202							//������빤��㲥

#define SUB_CL_CLUB_DISS_CLUB					3							//��ɢ����
#define CMD_LC_CLUB_DISS_CLUB					103							//��ɢ���᷵��

#define SUB_CL_CLUB_ALL_CLUB_INFO_LIST			4							//�����������б�
#define CMD_LC_CLUB_ALL_CLUB_INFO_LIST			104							//�����������б�
#define CMD_LC_CLUB_ALL_INFO_FINISH				204							//�����������б� ����

#define SUB_CL_CLUB_RANDOM_CLUB_LIST			5							//��ѯδ��Ա, �������������(���9��)
#define CMD_LC_CLUB_RANDOM_CLUB_LIST			105							//��ѯδ��Ա������������� ����
#define CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH		205							//��ѯδ��Ա������������� ����
#pragma endregion

#pragma region ���᷿�� ����|����|�޸�|�б�
#define SUB_CL_CLUB_ROOM_LIST					11							//����ù��᷿���б�
#define CMD_LC_CLUB_ROOM_LIST					111							//����ù��᷿���б� ����
#define CMD_LC_CLUB_ROOM_LIST_FINISH			211							//����ù��᷿���б� ����

#define SUB_CL_CLUB_ROOM_SETTING				13							//������������ 
#define CMD_LC_CLUB_ROOM_SETTING				113							//������������ ����

#define SUB_CG_CLUB_ROOM_CREATE					14							//��������
#define CMD_LC_CLUB_ROOM_CREATE					114							//�������� ����

#define SUB_CL_CLUB_ROOM_DISSOLVE				15							//��ɢ�������� 
#define CMD_LC_CLUB_ROOM_DISSOLVE				115							//��ɢ�������� ����
#pragma endregion

#pragma region  �������� ����|����|�б�
#define SUB_CG_CLUB_TABLE_LIST_TABLE			16							//����÷����������Ϣ
#define CMD_GC_CLUB_TABLE_LIST_TABLE			116							//����÷����������Ϣ ����

#define SUB_CG_USER_JOIN_TABLE_NO_PASS			19							//#��������,����Ҫ����, �����ٿ�ʼ -- ������Ⱥʹ��
#define SUB_CG_CLUB_CREATE_TABLE				21							//�������� 
#define CMD_GC_CLUB_CREATE_TABKE				121							//�������� ����

#define SUB_CL_CLUB_TABLE_DISSOLVE				18							//��ɢ�������� 
#define CMD_LC_CLUB_TABLE_DISSOLVE				118							//��ɢ�������� ����
#pragma endregion

#pragma region ������Ϣ���
#define SUB_CL_CLUB_NOTICE						21							//����Ȧ���� -- �ڲ���Ա�ɿ�
#define CMD_LC_CLUB_NOTICE						121							//����Ȧ���� ����
#define CMD_LC_CLUB_NOTICE_BDCAST				221							//����Ȧ���� �㲥

#define SUB_CL_CLUB_MESSAGE						22							//����Ȧ��� -- ���ⲿչʾ����Ϣ
#define CMD_LC_CLUB_MESSAGE						122							//����Ȧ��� ����  ע:����Ȧ��� ����㲥

#define SUB_CL_CLUB_CONTRIBUTE_FK				23							//���׷��� -- ���ⲿչʾ����Ϣ
#define CMD_LC_CLUB_CONTRIBUTE_FK				123							//���׷��� ����

#define SUB_CL_CLUB_AUTO_AGREE					24							//����Ȧ���� -- ��������Զ�ͬ��
#define CMD_LC_CLUB_AUTO_AGREE					124							//����Ȧ���� ����

#define SUB_CL_CLUB_CHAT						25							//����ALL 
#define CMD_LC_CLUB_CHAT						125							//����ALL ����
#define CMD_LC_CLUB_CHAT_BDCAST					225							//����ALL �㲥

#define SUB_CL_CLUB_MEMBER_MANAGER				31							//���빤�������
#define CMD_LC_CLUB_MEMBER_MANAGER				131							//���빤������� ����
#define CMD_LC_CLUB_MEMBER_MANAGER_FINISH		231							//���빤������� ����
#define CMD_LC_CLUB_DATA						331							//���������Ϣ

#define SUB_CL_CLUB_INVITE						32							//���������������Ȧ 
#define CMD_LC_CLUB_INVITE						132							//�������˼�������Ȧ ����
#define CMD_LC_CLUB_INVITE_REMIND				232							//�������˵�����

#define SUB_CL_CLUB_INVITE_RESULT				33							//�������˵Ļظ�
#define CMD_LC_CLUB_INVITE_RESULT				133							//�������˵Ļظ� ����

#define SUB_CL_CLUB_APPLICANT_LIST				34							//����Ȧ���������б�
#define CMD_LC_CLUB_APPLICANT_LIST				134							//����Ȧ���������б� ����
#define CMD_LC_CLUB_APPLICANT_LIST_FINISH		234							//����Ȧ���������б� ����

#define SUB_CL_CLUB_APPLICANT_RESULT			35							//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)
#define CMD_LC_CLUB_APPLICANT_RESULT			135							//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)

#define SUB_CL_CLUB_QUIT						36							//�û��˳�����Ȧ
#define CMD_LC_CLUB_QUIT						136							//�û����˳�����Ȧ ����

#define SUB_CL_CLUB_APPOINTMENT					37							//ְ������ 0��� 1��Ϊһ���Ա 2����Ϊ����Ա 
#define CMD_LC_CLUB_APPOINTMENT					137							//ְ������ ����
#define CMD_LC_CLUB_APPOINTMENT_NOTE			237							//ְ������ ����

#define SUB_CL_CLUB_INQUERY_LIST				38							//�������˲鿴�Լ��������б�
#define CMD_LC_CLUB_INQUERY_LIST				138							//�������˲鿴�Լ��������б� ����
#define CMD_LC_CLUB_INQUERY_LIST_FINISH			238							//�������˲鿴�Լ��������б� ����
#pragma endregion

#pragma region ����ս��
#define SUB_CL_CLUB_RECORD_LIST					41							//����ս��ͳ�� ��ѯ
#define CMD_LC_CLUB_RECORD_LIST					141							//����ս��ͳ�� ����
#define CMD_LC_CLUB_RECORD_FINISH				241							//����ս��ͳ�� ����
#pragma endregion

#endif