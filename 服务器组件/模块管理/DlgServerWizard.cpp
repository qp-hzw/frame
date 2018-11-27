#include "StdAfx.h"
#include "Resource.h"
#include "DlgServerWizard.h"
#include ".\dlgserverwizard.h"

#include "//www.45quyou.com/share\������\�������\WHService.h"
#include "//www.45quyou.com/share\������\�ں�����\TraceService.h"
#include "//www.45quyou.com/share\������\�������\WHIniData.h"
#include "..\..\ȫ�ֶ���\Struct.h"
#include "//www.45quyou.com/share\������\�������\WHEncrypt.h"

#pragma region DECLARE_MESSAGE_MAP
BEGIN_MESSAGE_MAP(CDlgServerWizardItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardItem1, CDlgServerWizardItem)
	ON_NOTIFY(NM_DBLCLK, IDC_MODULE_LIST, OnNMDblclkModuleList)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardItem2, CDlgServerWizardItem)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizard, CDialog)
	ON_BN_CLICKED(IDC_LAST, OnBnClickedLast)
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_FINISH, OnBnClickedFinish)
END_MESSAGE_MAP()

#pragma endregion

#pragma region CDlgServerWizardItem
//���캯��
CDlgServerWizardItem::CDlgServerWizardItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//���ñ���
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//��������
CDlgServerWizardItem::~CDlgServerWizardItem()
{
}

//��������
bool CDlgServerWizardItem::SaveItemData()
{
	//��������
	if ((m_hWnd!=NULL)&&(SaveInputInfo()==false))
	{
		return false;
	}

	return true;
}

//������
bool CDlgServerWizardItem::ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	if (m_hWnd==NULL) 
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

		//��������
		Create(m_lpszTemplateName,pParentWnd);
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

		//������Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//��ʾ����
	ShowWindow(SW_SHOW);

	return true;
}

//ȷ������
VOID CDlgServerWizardItem::OnOK()
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgServerWizardItem::OnCancel()
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//λ����Ϣ
VOID CDlgServerWizardItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}
#pragma endregion

#pragma region CDlgServerWizardItem1
/* *******************************************************************************
** CDlgServerWizardItem 1      
** ******************************************************************************/
//���캯��
CDlgServerWizardItem1::CDlgServerWizardItem1() : CDlgServerWizardItem(IDD_SERVER_WIZARD_1)
{
}

//�ؼ���
VOID CDlgServerWizardItem1::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROMPT, m_StaticPrompt);
	DDX_Control(pDX, IDC_MODULE_LIST, m_ModuleListControl);
}

//��ʼ������
BOOL CDlgServerWizardItem1::OnInitDialog()
{
	__super::OnInitDialog();

	//������Ϸģ���б�
	LoadDBModuleItem();

	return TRUE;
}

//��������
bool CDlgServerWizardItem1::SaveInputInfo()
{	
	POSITION Position=m_ModuleListControl.GetFirstSelectedItemPosition();
	tagGameGame * pGameGameItem = NULL;

	//��ȡѡ��
	if (Position!=NULL)
	{
		INT nListItem=m_ModuleListControl.GetNextSelectedItem(Position);

		DWORD *pGameID = (DWORD*) m_ModuleListControl.GetItemData(nListItem);
		DWORD dwGameID = *pGameID;
		pGameGameItem =  m_GameItemMap.SearchModuleInfo(dwGameID);
	}

	//ѡ���ж�
	if (pGameGameItem == NULL)
	{
		AfxMessageBox(TEXT("�����ȴ���Ϸ�б���ѡ����Ϸ���"),MB_ICONERROR);
		return false;
	}

	//��Ϸģ��
	m_GameServiceManager.CloseInstance();
	m_GameServiceManager.SetModuleCreateInfo(pGameGameItem->szDLLName,GAME_SERVICE_CREATE_NAME);

	//����ģ�飬CreateInstance��������Ϸdll
	if (m_GameServiceManager.CreateInstance()==false)
	{
		//������ʾ
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] �����������ʧ�ܣ�������Ϸ����ʧ��"),pGameGameItem->szGameName);

		//��ʾ��Ϣ
		AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//����ģ�� -- �˴��ſ�ʼʵ����m_pGameServiceOption �� m_pGameServiceAttrib
	m_pDlgServerWizard->SetWizardParameter(m_GameServiceManager.GetInterface(),NULL, pGameGameItem->szGameName);

	//����ģ�飨����dll��֮��, ���а汾У��  ����Ϸ�еĿ�ܰ汾 �� ��ʵ�Ŀ�ܰ汾
	DWORD realFrameVersion = Get_Framework_Version(PLATFORM_VERSION);
	DWORD subGameFrameVersion = m_pGameServiceAttrib->dwSubGameVersion;
	
	//TODONOW ��ʱȡ������ж� 
	/*
	if ( 0 != Compare_Dll_Framework(realFrameVersion, subGameFrameVersion))
	{
		TCHAR pszString2[512]=TEXT("");
		_sntprintf_s(pszString2,CountArray(pszString2),TEXT("dll����ʧ��, ��ܰ汾��ƥ��, realFrame: %ld; subGameFrame: %ld\n"),
					realFrameVersion,
					subGameFrameVersion);
		CTraceService::TraceString(pszString2,TraceLevel_Exception);

		return false; 
	}
	*/

	//m_pGameServiceOption��һ�θ�ֵ, ������Դ�����ݿ�
	m_pGameServiceOption->dwServerID = GetServerID();
	memcpy(m_pGameServiceOption->szServerName, pGameGameItem->szGameName, sizeof(m_pGameServiceOption->szServerName));//�˴���serverNameʵΪKindName
	m_pGameServiceOption->wGameServerPort = GetServerPort();
	memcpy(m_pGameServiceOption->szGameServerAddr, pGameGameItem->szGameServerAddr, sizeof(m_pGameServiceOption->szGameServerAddr));
	
	return true;
}
//�Զ���ȡServerID
DWORD CDlgServerWizardItem1::GetServerID()
{
	//added by WangChengQing ����ServerID
	//1. ��ȡ����Sql�����е�ServerID
	//2. �ж�(��ǰServerID��GameID) ��  ��ȡ����ServerID �Ա�
	//3. �ҵ� ����RoomID
	//4. ����RoomId +1
	//5. �������µ�ServerID
	CServerInfoManager ServerInfoManager;
	CServerItemManager ServerInfoBuffer;

	if (ServerInfoManager.LoadGameServerInfo(NULL,ServerInfoBuffer)==false)
	{
		//��ʾ��Ϣ
		AfxMessageBox(TEXT("����ServerIDʧ��"),MB_ICONERROR);
		return false;
	}

	//Ѱ��ͬGameID ����RoomID
	BYTE byMaxRoomID = 0;
	POSITION Position=ServerInfoBuffer.m_GameServerInfoMap.GetStartPosition();
	//ö��ģ��
	while (Position!=NULL)
	{
		//��ȡ����
		DWORD wServerID=0L;
		tagGameRoomItem * pGameServerInfo=NULL;
		ServerInfoBuffer.m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);

		//ѭ���ж�
		ASSERT(pGameServerInfo!=NULL);
		if (pGameServerInfo!=NULL)
		{
			DWORD dwGameID = (pGameServerInfo->dwServerID & 0xFFFFFF00);
			BYTE  byRoomID = pGameServerInfo->dwServerID & 0xFF;
			if((dwGameID == m_pGameServiceAttrib->dwGameID) && (byRoomID > byMaxRoomID))
			{
				byMaxRoomID = byRoomID;
			}
		}
	}
	byMaxRoomID += 1;
	DWORD dwServerID = (m_pGameServiceAttrib->dwGameID) | byMaxRoomID;

	return dwServerID;
}
//�Զ���ȡServerPort
WORD CDlgServerWizardItem1::GetServerPort()
{
	//added by WangChengQing ����ServerID
	//1. ��ȡ����Sql�����е�ServerID�Ķ˿�
	//2. ���˲����Ϲ���Ķ˿�
	//3. �ҵ� ��С��port �� ����port
	//4. ��С�� +1 ���� ����port -1
	//5. �������µ�port
	CServerInfoManager ServerInfoManager;
	CServerItemManager ServerInfoBuffer;

	if (ServerInfoManager.LoadGameServerInfo(NULL,ServerInfoBuffer)==false)
	{
		//��ʾ��Ϣ
		AfxMessageBox(TEXT("����ServerPortʧ��"),MB_ICONERROR);
		return false;
	}

	//Ѱ����� ��  ��С��ServerPort
	WORD wMinServerPort = _MAX_SERVER_PORT;
	WORD wMaxServerPort = _MIN_SERVER_PORT;

	POSITION Position=ServerInfoBuffer.m_GameServerInfoMap.GetStartPosition();
	//ö��ģ��
	while (Position!=NULL)
	{
		//��ȡ����
		DWORD wServerID=0L;
		tagGameRoomItem * pGameServerInfo=NULL;
		ServerInfoBuffer.m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);

		//ѭ���ж�
		if (pGameServerInfo!=NULL)
		{
			//����У�� port����ȷ�� continue
			if(pGameServerInfo->wGameServerPort < _MIN_SERVER_PORT
			|| pGameServerInfo->wGameServerPort > _MAX_SERVER_PORT)
			{
				continue;
			}
			//��ȡ��С��port
			wMinServerPort = (pGameServerInfo->wGameServerPort < wMinServerPort) ? pGameServerInfo->wGameServerPort : wMinServerPort;
			
			//��ȡ����port
			wMaxServerPort = (pGameServerInfo->wGameServerPort > wMaxServerPort) ? pGameServerInfo->wGameServerPort : wMaxServerPort;
		}
	}

	//����У�� -- ��֤�п�������
	if((wMinServerPort < _MIN_SERVER_PORT) || (wMaxServerPort > _MAX_SERVER_PORT) )
	{
		//��ʾ��Ϣ
		AfxMessageBox(TEXT("����ServerPortʧ��, �����쳣"),MB_ICONERROR);
		return false;
	}
	if( (wMinServerPort ==_MIN_SERVER_PORT) && (wMaxServerPort == _MAX_SERVER_PORT ))
	{
		//��ʾ��Ϣ
		AfxMessageBox(TEXT("����ServerPortʧ��, �˿�����"),MB_ICONERROR);
		return false;
	}

	WORD wServerPort = 0;
	//����ѡ�� �Ͷ˿�
	if(wMinServerPort > _MIN_SERVER_PORT)
	{
		wServerPort = wMinServerPort-1;
	}
	else
	{
		wServerPort = wMaxServerPort-1;
	}	

	return wServerPort;
}
//�����ؼ�
VOID CDlgServerWizardItem1::RectifyControl(INT nWidth, INT nHeight)
{
	//������ʾ
	if (m_StaticPrompt.m_hWnd!=NULL)
	{
		m_StaticPrompt.SetWindowPos(NULL,5,8,nWidth-10,12,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	//�����б�
	if (m_ModuleListControl.m_hWnd!=NULL)
	{
		m_ModuleListControl.SetWindowPos(NULL,5,28,nWidth-10,nHeight-28,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	return;
}

//�����б�
bool CDlgServerWizardItem1::LoadDBModuleItem()
{
	//������Ϣ
	if (m_ModuleInfoManager.LoadGameModuleInfo(m_GameItemMap)==true)
	{
		//�����б�
		m_ModuleListControl.DeleteAllItems();

		//��������
		POSITION Position=m_GameItemMap.m_GameModuleInfoMap.GetStartPosition();

		//ö��ģ��
		while (Position!=NULL)
		{
			//��ȡ����
			DWORD wModuleID=0L;
			tagGameGame * pGameGameItem=NULL;
			m_GameItemMap.m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameGameItem);

			//�����б�
			ASSERT(pGameGameItem!=NULL);
			if (pGameGameItem!=NULL) m_ModuleListControl.InsertModuleInfo(pGameGameItem);
		}
	}

	return false;
}

//˫���б�
VOID CDlgServerWizardItem1::OnNMDblclkModuleList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��������
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		//��������
		INT nListItem=((NMITEMACTIVATE *)pNMHDR)->iItem;
		DWORD *pGameID = (DWORD*) m_ModuleListControl.GetItemData(nListItem);
		DWORD dwGameID = *pGameID;
		tagGameGame * pGameGameItem =  m_GameItemMap.SearchModuleInfo(dwGameID);

		//Ͷ����Ϣ
		if (pGameGameItem->dwNativeVersion!=0L)
		{
			GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDC_NEXT,0),0);
		}

		return;
	}

	return;
}
#pragma endregion

#pragma region CDlgServerWizardItem2
/* *******************************************************************************
** CDlgServerWizardItem   2  
** ******************************************************************************/
//���캯��
CDlgServerWizardItem2::CDlgServerWizardItem2() : CDlgServerWizardItem(IDD_SERVER_WIZARD_2)
{
	//���ñ���
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
}

//�ؼ���
VOID CDlgServerWizardItem2::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//�����ؼ�
VOID CDlgServerWizardItem2::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//������Ϣ
VOID CDlgServerWizardItem2::OnSetFocus(CWnd * pNewWnd)
{
	__super::OnSetFocus(pNewWnd);

	return;
}
//��ʼ������
BOOL CDlgServerWizardItem2::OnInitDialog()
{
	__super::OnInitDialog();

	//����ؼ�
	InitCtrlWindow();

	/* Top: �������� */
	//��������
	//GameName
	TCHAR chGameName[LEN_GAME];
	memcpy(chGameName, m_pGameServiceOption->szServerName, LEN_GAME);//�˴���serverNameΪGameName
	//RoomId
	BYTE byRoomID = m_pGameServiceOption->dwServerID & 0xFF;
	//RoomName
	TCHAR chRoomName[LEN_SERVER] = TEXT("Errorδ�ҵ���������");	
	_sntprintf_s(chRoomName, LEN_SERVER, TEXT("%s��%d����"),chGameName, byRoomID);

	SetDlgItemText(IDC_SERVER_NAME,chRoomName);

	//����˿� -- �˴��˿ں���Ч������ʾ����
	if (m_pGameServiceOption->wGameServerPort!=0)
	{
		SetDlgItemInt(IDC_SERVER_PORT,m_pGameServiceOption->wGameServerPort);
	}
	else
	{
		TCHAR note[32] = TEXT("Error�˿ںŴ���");
		SetDlgItemText(IDC_SERVER_PORT,note);
	}

	return TRUE;
}

//��������
bool CDlgServerWizardItem2::SaveInputInfo()
{
	/* ��һ��: ���ݻ�ȡ */
	//��������
	CComboBox * pServerType=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	CIPAddressCtrl * pIPAddressCtrl=(CIPAddressCtrl *)GetDlgItem(IDC_DATABASE_ADDR);

	/* �������� */
	// ��������
	GetDlgItemText(IDC_SERVER_NAME,m_pGameServiceOption->szServerName,CountArray(m_pGameServiceOption->szServerName));
	// ����˿�
	m_pGameServiceOption->wGameServerPort=GetDlgItemInt(IDC_SERVER_PORT);

	/* �ڶ���: ����У�� */
	//���ݿ��ַ
	//�˿�����
	if(m_pGameServiceOption->wGameServerPort == 0)
	{
		AfxMessageBox(TEXT("Server�˿ںŲ���Ϊ�գ��������ݿ�"),MB_ICONERROR);
		return false;
	}
	//��������
	if (m_pGameServiceOption->szServerName[0]==0)
	{
		AfxMessageBox(TEXT("��Ϸ�������ֲ���Ϊ�գ���������Ϸ��������"),MB_ICONERROR);
		return false;
	}
	return true;
}

//����ؼ�
VOID CDlgServerWizardItem2::InitCtrlWindow()
{
	((CEdit *)GetDlgItem(IDC_SERVER_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SERVER_NAME))->LimitText(31);
	return;
}

//ȷ������
VOID CDlgServerWizardItem2::OnOK() 
{ 
	//Ͷ����Ϣ
	m_pDlgServerWizard->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgServerWizardItem2::OnCancel() 
{ 
	//Ͷ����Ϣ
	m_pDlgServerWizard->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//��������
bool CDlgServerWizardItem2::SaveItemData()
{
	//��������
	if ((m_hWnd!=NULL)&&(SaveInputInfo()==false))
	{
		return false;
	}

	return true;
}

//��ʾ����
bool CDlgServerWizardItem2::ShowOptionItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	if (m_hWnd==NULL) 
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

		//��������
		Create(m_lpszTemplateName,pParentWnd);
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

		//������Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//��ʾ����
	ShowWindow(SW_SHOW);

	return true;
}

#pragma endregion

#pragma region CDlgServerWizard
/* *******************************************************************************
** CDlgServerWizard 
** ******************************************************************************/
//���캯��
CDlgServerWizard::CDlgServerWizard() : CDialog(IDD_SERVER_WIZARD_MAIN)
{
	//���ñ���
	m_wActiveIndex=INVALID_WORD;
	m_pWizardItem[0]=&m_ServerWizardItem1;
	m_pWizardItem[1]=&m_ServerWizardItem2;

	//�ӿڱ���
	m_pIGameServiceManager=NULL;
	m_pIGameServiceCustomRule=NULL;

	//������Ϣ
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//��ʼ������
BOOL CDlgServerWizard::OnInitDialog()
{
	__super::OnInitDialog();

	//������Դ
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//������ -- ���÷���ʱ�� ֱ������CDlgServerWizardItem2
	ActiveWizardItem((m_pIGameServiceManager==NULL)?0:1);

	return FALSE;
}

//ȷ������
VOID CDlgServerWizard::OnOK()
{
	if ((m_wActiveIndex+1)<CountArray(m_pWizardItem))
	{
		OnBnClickedNext();
	}
	else
	{
		OnBnClickedFinish();
	}

	return;
}

//��������
bool CDlgServerWizard::CreateGameServer()
{
	//������Դ
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//���÷���
	if (DoModal()==IDOK)
	{
		return true;
	}

	return false;
}

//���ýӿ�
VOID CDlgServerWizard::SetWizardParameter(IGameServiceManager * pIGameServiceManager, tagGameServiceOption * pGameServiceOption, TCHAR* GameName)
{
	//��������
	if (pGameServiceOption==NULL)
	{
		for (WORD i=1;i<CountArray(m_pWizardItem);i++)
		{
			if ((m_pWizardItem[i]!=NULL)&&(m_pWizardItem[i]->m_hWnd!=NULL))
			{
				m_pWizardItem[i]->DestroyWindow();
			}
		}
	}

	//���ñ���
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	//��ȡ����Ϸ�е�Attribute
	pIGameServiceManager->GetServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);

	//���ýӿ�
	m_pIGameServiceManager=pIGameServiceManager;

	//�������
	if (m_hWnd!=NULL)
	{
		//��Ϸ����
		if(GameName != NULL)
		{
			SetDlgItemText(IDC_GAME_NAME, GameName);//�˴���serverName��ΪKindName
		}

		//GameID
		SetDlgItemInt(IDC_GAME_ID,m_ModuleInitParameter.GameServiceAttrib.dwGameID);
		
		//����Ϸ   ����Ϸ�汾 
		SetDlgItemInt(IDC_SUBGAME_VERSION,m_ModuleInitParameter.GameServiceAttrib.dwSubGameVersion);
	}

	//���ù���
	if (pGameServiceOption==NULL)
	{
		//��������
		m_pIGameServiceManager->RectifyParameter(m_ModuleInitParameter.GameServiceOption);
		
		//����GameID
		m_ModuleInitParameter.GameServiceOption.dwGameID = m_ModuleInitParameter.GameServiceAttrib.dwGameID;
	}
	else
	{
		//��������
		CopyMemory(&m_ModuleInitParameter.GameServiceOption,pGameServiceOption,sizeof(tagGameServiceOption));
	}

	return;
}

//������
bool CDlgServerWizard::ActiveWizardItem(WORD wIndex)
{
	//�ж�״̬
	if (m_wActiveIndex==wIndex)
	{
		m_pWizardItem[m_wActiveIndex]->SetFocus();
		return true;
	}

	//�������
	CDlgServerWizardItem * pItemWizard=NULL;
	if (m_wActiveIndex<CountArray(m_pWizardItem))
	{
		//���ñ���
		pItemWizard=m_pWizardItem[m_wActiveIndex];

		//�������� -- ������һ��ʱ��,��ǰ����ʧ���ˡ� �򱨴�			
		if ((wIndex>m_wActiveIndex)&&(m_pWizardItem[m_wActiveIndex]->SaveItemData()==false)) return false;
	}

	//��ȡλ��
	CRect rcItemRect;
	GetDlgItem(IDC_ITEM_FRAME)->GetWindowRect(rcItemRect);
	ScreenToClient(&rcItemRect);

	//��������
	m_pWizardItem[wIndex]->m_pDlgServerWizard=this;
	m_pWizardItem[wIndex]->m_pGameServiceAttrib=&m_ModuleInitParameter.GameServiceAttrib;
	m_pWizardItem[wIndex]->m_pGameServiceOption=&m_ModuleInitParameter.GameServiceOption;

	//��������
	m_wActiveIndex=wIndex;
	m_pWizardItem[m_wActiveIndex]->ShowWizardItem(rcItemRect,this);

	//��������
	m_pWizardItem[m_wActiveIndex]->SetFocus();
	if (pItemWizard!=NULL) pItemWizard->ShowWindow(SW_HIDE);

	//�������
	CButton * pButtonLast=(CButton * )GetDlgItem(IDC_LAST);
	CButton * pButtonNext=(CButton * )GetDlgItem(IDC_NEXT);
	CButton * pButtonFinish=(CButton * )GetDlgItem(IDC_FINISH);

	//���Ƚ���
	pButtonNext->EnableWindow(((m_wActiveIndex+1)<CountArray(m_pWizardItem))?TRUE:FALSE);
	pButtonFinish->EnableWindow(((m_wActiveIndex+1)==CountArray(m_pWizardItem))?TRUE:FALSE);
	pButtonLast->EnableWindow(((m_wActiveIndex>1)||((m_ModuleInitParameter.GameServiceOption.dwServerID==0)&&(m_wActiveIndex>0)))?TRUE:FALSE);

	//�������
	TCHAR szTitle[128]=TEXT("");
	_sntprintf_s(szTitle,CountArray(szTitle),TEXT("���������� --- [ ���� %d ]"),m_wActiveIndex+1);
	
	//���ñ���
	SetWindowText(szTitle);

	return true;
}

//��һ��
VOID CDlgServerWizard::OnBnClickedLast()
{
	//Ч�����
	ASSERT(m_wActiveIndex>0);
	ASSERT(m_wActiveIndex<CountArray(m_pWizardItem));

	//�л�ҳ��
	ActiveWizardItem(m_wActiveIndex-1);

	return;
}

//��һ��
VOID CDlgServerWizard::OnBnClickedNext()
{
	//Ч�����
	ASSERT((m_wActiveIndex+1)<CountArray(m_pWizardItem));

	//����ҳ��
	ActiveWizardItem(m_wActiveIndex+1);

	return;
}

//��ɰ�ť
VOID CDlgServerWizard::OnBnClickedFinish()
{
	//��������
	ASSERT(m_wActiveIndex>=1);
	if (m_pWizardItem[m_wActiveIndex]->SaveItemData()==false) return;

	//��������
	CServerInfoManager ServerInfoManager;
	
	//���뷿�䣨���ӶԻ�������ϻ�ȡ������д�����ݿ⣩
	if (ServerInfoManager.InsertGameServer(&m_ModuleInitParameter.GameServiceOption)==false) 
	{
		CTraceService::TraceString(TEXT("��������ʧ��"), TraceLevel_Normal);
		return;
	}
	else
		CTraceService::TraceString(TEXT("��������ɹ�"), TraceLevel_Normal);

	//�رմ���
	EndDialog(IDOK);

	return;
}

#pragma endregion