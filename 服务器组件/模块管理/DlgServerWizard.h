#ifndef DLG_SERVER_WIZARD_HEAD_FILE
#define DLG_SERVER_WIZARD_HEAD_FILE

#pragma once

#include "ModuleListControl.h"
#include "ModuleInfoManager.h"
#include "ServerInfoManager.h"
#include "ModuleManagerHead.h"


/* *******************************************************************************
** 简要描述:  该类组  对应  '创建房间'按钮之后的对话框组
** 详细描述:  1. 游戏列表
**            2. 房间信息
** 修 改 人:  WangChengQing
** 修改记录:  1. 修复加载dll问题, 使之适应最新版本
**            
** ******************************************************************************/

//类说明
class CDlgServerWizard;

//向导基类 -- CDlgServerWizardItem1 和 CDlgServerWizardItem2的基类
class MODULE_MANAGER_CLASS CDlgServerWizardItem : public CDialog
{
	//友元定义
	friend class CDlgServerWizard;

	//变量定义
protected:
	CDlgServerWizard *				m_pDlgServerWizard;					//向导指针
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置

	//函数定义
protected:
	//构造函数
	CDlgServerWizardItem(UINT nIDTemplate);
	//析构函数
	virtual ~CDlgServerWizardItem();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo()=NULL;
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//功能函数
private:
	//保存数据
	bool SaveItemData();
	//创建向导
	bool ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd);

	//消息函数
private:
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};

//游戏选择 -- 创建房间第一级页面  CDlgServerWizard控件 选择加载游戏
class MODULE_MANAGER_CLASS CDlgServerWizardItem1 : public CDlgServerWizardItem
{
	//列表变量
protected:
	CStatic							m_StaticPrompt;						//提示控件
	CGameItemMap					m_GameItemMap;						//模块信息
	CModuleListControl				m_ModuleListControl;				//模块列表
	CModuleInfoManager				m_ModuleInfoManager;				//模块管理

	//服务组件
protected:
	CGameServiceManagerHelper		m_GameServiceManager;				//游戏模块

	//函数定义
public:
	//构造函数
	CDlgServerWizardItem1();
	//析构函数
	virtual ~CDlgServerWizardItem1(){};

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数（初始化游戏组件列表）
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//功能函数
protected:
	//加载模块
	bool LoadDBModuleItem();
	//自动获取ServerID
	DWORD GetServerID();
	//自动获取ServerPort
	WORD GetServerPort();
	//控件消息
protected:
	//双击列表
	VOID OnNMDblclkModuleList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//房间配置 -- 创建房间第二级页面  CDlgServerWizard控件 游戏配置界面
class MODULE_MANAGER_CLASS CDlgServerWizardItem2 : public CDlgServerWizardItem
{
	//函数定义
public:
	//构造函数
	CDlgServerWizardItem2();
	//析构函数
	virtual ~CDlgServerWizardItem2(){};

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//功能函数
private:
	//保存数据
	bool SaveItemData();
	//显示配置
	bool ShowOptionItem(const CRect & rcRect, CWnd * pParentWnd);

	//功能函数
protected:
	//构造控件
	VOID InitCtrlWindow();

	//控件消息
protected:
	//焦点消息
	VOID OnSetFocus(CWnd * pNewWnd);

	DECLARE_MESSAGE_MAP()
};

//创建房间 主Dlg
class MODULE_MANAGER_CLASS CDlgServerWizard : public CDialog
{
	//友元定义
	friend class CDlgServerWizardItem2;

	//变量定义
protected:
	WORD							m_wActiveIndex;						//活动索引
	CDlgServerWizardItem *			m_pWizardItem[2];					//向导指针

	//配置信息
public:
	tagModuleInitParameter			m_ModuleInitParameter;				//配置参数

	//控件变量
protected:
	CDlgServerWizardItem1			m_ServerWizardItem1;				//设置步骤
	CDlgServerWizardItem2			m_ServerWizardItem2;				//设置步骤

	//接口变量
protected:
	IGameServiceManager *			m_pIGameServiceManager;				//服务管理
	IGameServiceCustomRule *		m_pIGameServiceCustomRule;			//自定配置

	//函数定义
public:
	//构造函数
	CDlgServerWizard();
	//析构函数
	virtual ~CDlgServerWizard(){};

	//重载函数
public:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();

	//配置函数
public:
	//创建房间
	bool CreateGameServer();
	//设置配置
	VOID SetWizardParameter(IGameServiceManager * pIGameServiceManager, tagGameServiceOption * pGameServiceOption, TCHAR* GameName = NULL);

	//辅助函数
private:
	//激活向导
	bool ActiveWizardItem(WORD wIndex);

	//消息映射
public:
	//上一步
	VOID OnBnClickedLast();
	//下一步
	VOID OnBnClickedNext();
	//完成按钮
	VOID OnBnClickedFinish();

	DECLARE_MESSAGE_MAP()
};

#endif