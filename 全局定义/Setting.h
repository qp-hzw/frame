#ifndef DEFINE_HEAD_FILE_FRAME
#define DEFINE_HEAD_FILE_FRAME

//��½����ַ(����ip, ��Э������¼ʹ��) ���Զ���ȡ
//��Ϸ����ַ(����ip; �����ݿ�У��ʹ��) ���Զ���ȡ
//Э���� �����ֶ�ָ��
/***********************************************22222222222222********************************************/
//���Է�����
const  BYTE		_MYSTERY				=		 2;				   
const  TCHAR    _CPD_SERVER_ADDR[32]    =        TEXT("127.0.0.1");			//Э���� -- �����ֶ�ָ��


/***********************************************22222222222222********************************************
//����ׯ������
const  BYTE		_MYSTERY				=		 2;				   //����ׯ
const  TCHAR    _CPD_SERVER_ADDR[32]    =        TEXT("120.27.235.52");		//Э����
*/


/***********************************************Э���� ��½��  ��Ϸ�� �˿�********************************************/
/* �������˿����� */
const WORD _MIN_PORT		=           8600;                    			//���������ŵ���С�˿� -- [min�� min+9]Ϊ����Ԥ��
const WORD _MAX_PORT		=           8900;                         	    //���������ŵ����˿�

const WORD  PORT_LOGON		=			_MIN_PORT;							//��½�������˿�
const WORD  PORT_CENTER		=			_MIN_PORT+1;						//Э���������˿�
const WORD  PORT_MANAGER	=			_MIN_PORT+2;						//����˿� (δʹ��)

const WORD _MIN_SERVER_PORT =           _MIN_PORT+10;                       //��С��ServerPort
const WORD _MAX_SERVER_PORT =           _MAX_PORT;                          //����ServerPort

#endif