#ifndef STR_CMD_LONGON_HEAD_FILE
#define STR_CMD_LONGON_HEAD_FILE

#pragma region �����ṹ��
////�����ϵ������Ϣ
struct tagClubTableUserInfo
{
	DWORD	dwUserID;									//���ID
	TCHAR	szUserName[LEN_NICKNAME];					//�������
	BYTE	bySex;										//�Ա�
	WORD	wLevel;										//�ȼ�
	TCHAR	szHeadUrl[LEN_HEAD_URL];							//ͷ���ַ

	BYTE	byClubRole;									//���ְ�� 0 Ⱥ��; 1����; 2һ���Ա
	BYTE	byClubReputation;							//����ڹ����е������ȼ�
};

//��������
struct STR_CMD_LC_LIST_ROOM_ONLINE
{
	DWORD	dwServerID;						//����ID
	DWORD	dwOnlineCount;					//����
};

//ͨ�ô�����ʾ
struct STR_SUB_CL_COMMON_ERROR
{
	TCHAR szMsg[20];            //ͨ�ô�����ʾ
};
#pragma endregion

#pragma region MDM_LOGON ��¼ģ��
//�ο͵�¼
struct STR_SUB_CL_LOGON_TEMP
{
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	DWORD							dwProxyID;							//����ID
};

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
	DWORD                           dwResultCode;							//�����ʶ
	TCHAR							szDescribeString[LEN_MESSAGE_DESCRIBE];	//������Ϣ

	tagUserInfo                     useInfo;                                //�����Ϣ 
	DWORD							dwOffLineGameID;					    //����������GameID, �ǿձ�ʾ�Ƕ�������
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

#pragma region MDM_SERVICE �û�����
//��ע, ��˿ 
struct STR_SUB_CL_SERVICE_FLOWER
{
	DWORD   dwTargetID;  //�鿴�����useID
};

//��ע, ��˿ 
struct STR_CMD_LC_SERVICE_FLOWER_P
{
	DWORD							dwUserID;							//�û�ID
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szHeadUrl[LEN_HEAD_URL];			//ͷ���ַ
	DWORD                           dwLoveValue;                        //����ֵ
};
//��ע, ��˿ 
struct STR_CMD_LC_SERVICE_FLOWER
{
	DWORD   dwFolwerNum;  //�ҵĹ�ע
	DWORD   dwFolwingNum; //�ҵķ�˿
	BYTE    cbIsGuanzhu;  //1��ʾ��ע��ť; ��������ʾ��ע��ť

	STR_CMD_LC_SERVICE_FLOWER_P folwingInfo[5];
};

//��ע, ȡ����ע
struct STR_SUB_CL_SERVICE_FLOWER_ACT
{
	DWORD   dwTargetID;  //�鿴�����useID
	BYTE    cbMask;      //1��ע, 2ȡ����ע
};
//��ע�� ȡ����ע ����
struct STR_CMD_LC_SERVICE_FLOWER_ACT
{
	BYTE    cbResult;    //0�ɹ��� ����ʧ��
	BYTE    cbMask;      //1��ע, 2ȡ����ע
};


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
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�	
	TCHAR							szHeadImageUrl[LEN_HEAD_URL];		//ͷ���ַ
	TCHAR							szSignature[LEN_MY_SIGNATURE];		//����ǩ��
	//��ϵ��ʽ
	TCHAR							szRealName[LEN_IDENTITY_NAME];		//��ʵ����
	TCHAR							szIDCardNum[LEN_IDENTITY_NUM];		//���֤��
	TCHAR							szPhoneNum[LEN_MOBILE_PHONE];		//���ֻ���
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

};
//���ս�� ���������Ϣ
struct RecordDjPlayerInfo
{
	DWORD					dwUserID;					//ID
	TCHAR					szName[LEN_NICKNAME];		//����ǳ�
	TCHAR					szHeadUrl[LEN_HEAD_URL];	//ͷ���ַ
	SCORE					llScore;					//����ܷ�
};
//���ս�� ����
struct STR_CMD_LC_SERVICE_PURE_RECORD_LIST
{
	DWORD					dwTableID;					//����id
	WORD					wGameMode;					//�������ͣ�0����1������2��3����4���ᣩ
	WORD					wGameCount;					//�ܾ���
	WORD					wPlayerCount;				//�����
	WORD					wKindID;					//��Ϸ����
	SYSTEMTIME				szTime;						//ʱ��
	TCHAR					szOnlyID[32];				//���Ψһ��ʶ
	RecordDjPlayerInfo		Info[MAX_CHAIR];			//�����Ϣ
};

//С��ս�� ��ѯ
struct STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST
{
	TCHAR					szOnlyID[32];				//���Ψһ��ʶ
};
//С��ս�� ���������Ϣ
struct RecodeXjPlayerInfo
{
	DWORD					dwUserID;					//ID
	TCHAR					szName[LEN_NICKNAME];		//����ǳ�
	TCHAR					szHeadUrl[LEN_HEAD_URL];	//ͷ���ַ
	SCORE					llScore;					//��ҵ÷�
	WORD					wIdentity;					//���  ��0ũ�� 1������
};
//С��ս�� ����
struct STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST
{
	DWORD					dwTableID;
	WORD					wCurCount;					//��ǰ����
	SYSTEMTIME				szTime;						//ʱ��
	WORD					wPlayerCount;				//�����
	TCHAR					szOnlyID[32];				//Ψһ��ʶ
	RecodeXjPlayerInfo		Info[MAX_CHAIR];			//�����Ϣ
};

//С��¼��ط�
struct STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK
{
	WORD					wCurCount;					//����
	TCHAR					szOnlyID[32];				//Ψһ��ʶ
};
//С��¼�� ���������Ϣ
struct RecodePlayerInfo
{
	DWORD					dwUserID;					//ID
	TCHAR					szName[LEN_NICKNAME];		//����ǳ�
	TCHAR					szHeadUrl[LEN_HEAD_URL];	//ͷ���ַ
	SCORE					llScore;					//��ҵ÷�
	WORD					wChairID;
};
//С��¼��طŷ���
struct STR_CMD_LC_SERVICE_XJ_RECORD_PLAYBACK
{
	BYTE					cbRecordData[LEN_MAX_RECORD_SIZE/4];	//¼������
	BYTE					cbCurCount;								//
	BYTE					cbAllCount;									
	WORD					wKindID;
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

#pragma region MDM_SHOP �̳ǵ���
//�̳ǹ���
struct STR_SUB_CL_SHOP_BUY
{
	DWORD dwGoodsID;							//��ƷID
	DWORD dwGoodsNum;                           //��Ʒ����
	BYTE byTreasureType;						//�Ƹ�����
};

//�̳ǹ��򷵻�
struct STR_CMD_LC_SHOP_BUY_RESULT
{
	BYTE  byResult;        //���ؽ�� 0�ɹ��� ����ʧ��
};

//������Ʒ��ѯ ����
struct STR_CMD_LC_BAG_RESULT
{
	DWORD dwGoodsID;		//��ƷID
	DWORD dwGoodsNum;		//��Ʒ����
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
	TCHAR							szIPAddr[LEN_IP_ADDR];					//ip��ַ
	DWORD							dwPort;									//ip�˿�

};
#pragma endregion

#endif