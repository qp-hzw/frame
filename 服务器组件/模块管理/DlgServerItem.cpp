#include "Stdafx.h"
#include "Resource.h"
#include "DlgServerItem.h"

//////////////////////////////////////////////////////////////////////////////////

//�б�����
#define LIST_STYTE LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_FLATSB

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerItem, CDialog)

	//��ť��Ϣ
	ON_BN_CLICKED(IDC_LOAD_SERVER, OnBnClickedLoadServer)
	ON_BN_CLICKED(IDC_RELOAD_LIST, OnBnClickedReLoadList)
	ON_BN_CLICKED(IDC_DELETE_SERVER, OnBnClickedDeleteServer)

	//�ؼ���Ϣ
	ON_NOTIFY(NM_DBLCLK, IDC_SERVER_LIST, OnNMDblclkServerList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SERVER_LIST, OnLvnItemChangedServerList)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerItem::CDlgServerItem() : CDialog(IDD_SERVER_ITEM)
{
	//ģ�����
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//��������
CDlgServerItem::~CDlgServerItem()
{
}

//�ؼ���
VOID CDlgServerItem::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_LIST, m_ServerListControl);
}

//��Ϣ����
BOOL CDlgServerItem::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//��ʼ������
BOOL CDlgServerItem::OnInitDialog()
{
	__super::OnInitDialog();

	//������Դ
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//���ط���
	LoadDBServerItem();

	return TRUE;
}

//ȷ����Ϣ
VOID CDlgServerItem::OnOK()
{
	return;
}

//�򿪷���
bool CDlgServerItem::OpenGameServer()
{
	//������Դ
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//���÷���
	if (DoModal()==IDC_LOAD_SERVER)
	{
		return true;
	}

	return false;
}

//�򿪷���
bool CDlgServerItem::OpenGameServer(DWORD dwServerID)
{
	//��������
	tagGameRoomItem tagGameRoomItem;

	//������ʶ
	TCHAR szMachineID[LEN_MACHINE_ID] = { 0 };
	CWHService::GetMachineID(szMachineID);

	//���ط���
	CServerInfoManager ServerInfoManager;
	if (ServerInfoManager.LoadGameServerInfo(szMachineID,dwServerID,tagGameRoomItem)==false)
	{
		return false;
	}

	//��ȡ����
	if (GetModuleInitParameter(&tagGameRoomItem,true)==false)
	{
		return false;
	}

	return true;
}

//�����б�
bool CDlgServerItem::LoadDBServerItem()
{
	//�����б�
	m_ServerListControl.DeleteAllItems();

	//���ð�ť
	GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(FALSE);

	//������ʶ
	TCHAR szMachineID[LEN_MACHINE_ID] = { 0 };
	//CWHService::GetMachineID(szMachineID);

	//������Ϣ
	CServerInfoManager ServerInfoManager;
	if (ServerInfoManager.LoadGameServerInfo(szMachineID,m_ServerItemMap)==true)
	{
		//��������
		POSITION Position=m_ServerItemMap.m_GameServerInfoMap.GetStartPosition();

		//ö��ģ��
		while (Position!=NULL)
		{
			//��ȡ����
			DWORD dwServerID=0L;
			tagGameRoomItem * pGameServerInfo=NULL;
			m_ServerItemMap.m_GameServerInfoMap.GetNextAssoc(Position,dwServerID,pGameServerInfo);

			//�����б�
			ASSERT(pGameServerInfo!=NULL);
			if (pGameServerInfo!=NULL) m_ServerListControl.InsertServerInfo(pGameServerInfo);
		}

		return true;
	}

	return false;
}

//���°�ť
bool CDlgServerItem::UpdateControlStatus()
{
	//��ȡѡ��
	POSITION Position=m_ServerListControl.GetFirstSelectedItemPosition();

	//�����б�
	if (Position!=NULL)
	{
		//��ȡ����
		INT nListItem=m_ServerListControl.GetNextSelectedItem(Position);
		DWORD *pServerID = (DWORD*) m_ServerListControl.GetItemData(nListItem);
		DWORD dwServerID = *pServerID;
		tagGameRoomItem * pGameServerInfo =  m_ServerItemMap.SearchServerInfo(dwServerID);

		//���ð�ť
		if (pGameServerInfo->dwNativeVersion!=0L)
		{
			GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(TRUE);
		}
	}
	else
	{
		//���ð�ť
		GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(FALSE);
	}

	return true;
}

//��ȡ����
bool CDlgServerItem::GetModuleInitParameter(tagGameRoomItem * pGameServerInfo, bool bAutoMode)
{
	//Ч�����
	if (pGameServerInfo==NULL) return false;

	//��Ϸģ��
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(pGameServerInfo->szDLLName,GAME_SERVICE_CREATE_NAME);

	//�����ж�
	if (pGameServerInfo->dwNativeVersion==0)
	{
		return false;
	}

	//����ģ��
	if (GameServiceManager.CreateInstance()==false)
	{
		return false;
	}

	//ģ������
	GameServiceManager->GetServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);

	//������־
	m_ModuleInitParameter.GameServiceOption.dwServerID=pGameServerInfo->dwServerID;
	lstrcpyn(m_ModuleInitParameter.GameServiceOption.szServerName,pGameServerInfo->szServerName,LEN_SERVER);
	//��Ϸ���˿�
	m_ModuleInitParameter.GameServiceOption.wGameServerPort=pGameServerInfo->wGameServerPort;

	//��Ϸ����ַ
	lstrcpyn(m_ModuleInitParameter.GameServiceOption.szGameServerAddr,pGameServerInfo->szGameServerAddr,LEN_DB_ADDR);

	return true;
}

//������Ϸ����
VOID CDlgServerItem::OnBnClickedLoadServer()
{
	//��ȡѡ��
	POSITION Position=m_ServerListControl.GetFirstSelectedItemPosition();
	
	//��ȡ����
	INT nListItem=m_ServerListControl.GetNextSelectedItem(Position);
	DWORD *pServerID = (DWORD*) m_ServerListControl.GetItemData(nListItem);
	DWORD dwServerID = *pServerID;
	tagGameRoomItem * pGameServerInfo =  m_ServerItemMap.SearchServerInfo(dwServerID);

	//���ñ���
	if ((pGameServerInfo!=NULL)&&(GetModuleInitParameter(pGameServerInfo,false)==true))
	{
		EndDialog(IDC_LOAD_SERVER);
	}

	return;
}

//ˢ���б�
VOID CDlgServerItem::OnBnClickedReLoadList()
{
	//�����б�
	LoadDBServerItem();

	return;
}

//ɾ������
VOID CDlgServerItem::OnBnClickedDeleteServer()
{
	//��ȡѡ��
	ASSERT(m_ServerListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_ServerListControl.GetFirstSelectedItemPosition();
	
	//��ȡ����
	INT nListItem=m_ServerListControl.GetNextSelectedItem(Position);
	DWORD *pServerID = (DWORD*) m_ServerListControl.GetItemData(nListItem);
	DWORD dwServerID = *pServerID;
	tagGameRoomItem * pGameServerInfo =  m_ServerItemMap.SearchServerInfo(dwServerID);

	//���ñ���
	if (pGameServerInfo!=NULL)
	{
		//������ʾ
		TCHAR szString[128]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("ȷʵҪɾ�� [ %s ] ��Ϸ������"),pGameServerInfo->szServerName);

		//��ʾ��Ϣ
		if (AfxMessageBox(szString,MB_ICONQUESTION|MB_YESNO)!=IDYES) return;

		//ɾ������
		CServerInfoManager ServerInfoManager;
		if (ServerInfoManager.DeleteGameServer(pGameServerInfo->dwServerID)==false) return;

		//ɾ���б�
		m_ServerListControl.DeleteItem(nListItem);
		m_ServerItemMap.DeleteServerInfo(pGameServerInfo->dwServerID);

		//�����б�
		if (m_ServerListControl.GetItemCount()>0)
		{
			//���ñ���
			INT nItemCount=m_ServerListControl.GetItemCount();
			INT nNextItem=(nListItem>=nItemCount)?(nItemCount-1):nListItem;

			//����ѡ��
			m_ServerListControl.SetItemState(nNextItem,LVIS_SELECTED,LVIS_SELECTED);
		}

		//���¿���
		UpdateControlStatus();
	}

	return;
}

//˫���б�
VOID CDlgServerItem::OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��������
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		OnBnClickedLoadServer();
	}

	return;
}

//ѡ��ı�
VOID CDlgServerItem::OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//���¿���
	UpdateControlStatus();

	return;
}

//////////////////////////////////////////////////////////////////////////////////
