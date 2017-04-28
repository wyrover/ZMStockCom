// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"

#ifndef WIN64
/// 定义是否同步处理数据
#define	ZM_TDXSERVER_SYNC
#endif

/// 定义模拟盘
#define	ZM_TDXSERVER_DEMO

/// 华福证券版
//#define	ZM_BROKER_HFZQ
/// 中信证券版
//#define	ZM_BROKER_ZXZQ

void CMainDlg::AdviseTradeClient(int nIndex)
{
	if(NULL != m_spiTradeClientEvent[nIndex])
		return;
	m_nTradeEventCookie[nIndex] = NULL;
	this->m_spiTradeClientEvent[nIndex] = new CComObject <CStockComEvent>;
	ATLASSERT(this->m_spiTradeClientEvent[nIndex]);
	if(NULL == this->m_spiTradeClientEvent[nIndex])
		return;	
	/// 设置回调参数，用于事件发生时匹配是哪个对象侦听的
	m_spiTradeClientEvent[nIndex]->SetCallPara(this->m_hWnd,nIndex);
	//建立事件连接
	HRESULT hRet = AtlAdvise(m_spiTrade[nIndex],this->m_spiTradeClientEvent[nIndex]->GetUnknown(),
		DIID__ITradeEvents,&this->m_nTradeEventCookie[nIndex]);
}

void CMainDlg::UnAdviseTradeClient(int nIndex)
{
	if(0 == m_nTradeEventCookie[nIndex])
		return;
	AtlUnadvise(m_spiTrade[nIndex],DIID__ITradeEvents,m_nTradeEventCookie[nIndex]);
	m_nTradeEventCookie[nIndex] = 0;
	m_spiTradeClientEvent[nIndex] = NULL;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	m_wndTradeList.Attach(GetDlgItem(IDC_COMBO_TRADETYPE).m_hWnd);
	int nItemIndex = m_wndTradeList.AddString(L"资金");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_CAPITAL);
	nItemIndex = m_wndTradeList.AddString(L"股份");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_STOCK);
	nItemIndex = m_wndTradeList.AddString(L"当日委托");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_TODAYORDER);
	nItemIndex = m_wndTradeList.AddString(L"当日成交");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_TODAYTRADE);
	nItemIndex = m_wndTradeList.AddString(L"当日可撤委托");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_TODAYCANREVOKE);
	nItemIndex = m_wndTradeList.AddString(L"股东代码");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_SHAREHOLDERCODE);
	nItemIndex = m_wndTradeList.AddString(L"融资余额");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_BALANCEOFFINANCING);
	nItemIndex = m_wndTradeList.AddString(L"融券余额");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_BORROWFUNDLEFT);
	nItemIndex = m_wndTradeList.AddString(L"可融证券");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_CANBORROWFUND);
	nItemIndex = m_wndTradeList.AddString(L"可申购新股");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_CANNEWSTOCK);
	nItemIndex = m_wndTradeList.AddString(L"新股申购额度");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_CANNEWSTOCKLIMIT);
	nItemIndex = m_wndTradeList.AddString(L"配号");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_ASSIGNNUMBER);
	nItemIndex = m_wndTradeList.AddString(L"中签");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_BALLOT);
	m_wndTradeList.SetCurSel(0);

	m_wndHistoryList.Attach(GetDlgItem(IDC_COMBO_HISLIST).m_hWnd);
	nItemIndex = m_wndHistoryList.AddString(L"历史委托");
	m_wndHistoryList.SetItemData(nItemIndex,HISORDERTYPE_TRUST);
	nItemIndex = m_wndHistoryList.AddString(L"历史成交");
	m_wndHistoryList.SetItemData(nItemIndex,HISORDERTYPE_TRADE);
	nItemIndex = m_wndHistoryList.AddString(L"交割单");
	m_wndHistoryList.SetItemData(nItemIndex,HISORDERTYPE_DELIVERYORDER);
	m_wndHistoryList.SetCurSel(0);

	m_nTradeEventCookie[0] = 0;
	m_spiTradeClientEvent[0] = NULL;
	m_nTradeEventCookie[1] = 0;
	m_spiTradeClientEvent[1] = NULL;

	/// 创建COM对象前请先注册，管理员权限执行ZMStockCom.exe /RegServer(为当前账号注册，如果需要，请注册为每一个用户，参数是RegServerPerUser)
	/// 卸载执行ZMStockCom.exe /UnregServer(为当前账号注册，如果注册成了每一个用户，参数是UnregServerPerUser)
	/// 如果创建失败，请先检查是否注册成功，检查运行所需要的依赖DLL是否都和本程序在一个目录
	HRESULT hRet = m_spiTrade[0].CreateInstance(__uuidof(StockTrade));
	ATLASSERT(m_spiTrade[0]);
	if(NULL != m_spiTrade[0])
	{
		/// 建立事件连接
		AdviseTradeClient(0);

		/// 启用调试日志输出
		hRet = m_spiTrade[0]->put_EnableLog(VARIANT_TRUE);
		/// 初始化使用环境，单个实例只需要调用一次，根据客户端版本号，最大连接数(默认为1)，最好和设置的服务器数量匹配
		/// 为提高执行效率，实现为异步操作，需要接收事件通知得到初始化结果
		m_spiTrade[0]->Init(CComBSTR(L"6.4"),1);
	}

#ifdef ZM_BROKER_HFZQ
	this->GetDlgItem(IDC_EDIT_TRADESERVERADDR).SetWindowText(L"tdxwt.hfzq.com.cn");
	this->GetDlgItem(IDC_EDIT_TRADESERVERPORT).SetWindowText(L"8081");
	this->GetDlgItem(IDC_EDIT_YYBID).SetWindowText(L"1");
	/// 设置实际的登录账号和交易账号
	this->GetDlgItem(IDC_EDIT_LOGINACCOUNTNO).SetWindowText(L"");
	this->GetDlgItem(IDC_EDIT_TRADEACCOUNT).SetWindowText(L"");
#endif
#ifdef ZM_BROKER_ZXZQ
	this->GetDlgItem(IDC_EDIT_TRADESERVERADDR).SetWindowText(L"180.153.18.180");
	this->GetDlgItem(IDC_EDIT_TRADESERVERPORT).SetWindowText(L"7708");
	this->GetDlgItem(IDC_EDIT_YYBID).SetWindowText(L"1");
	/// 设置实际的登录账号和交易账号
	this->GetDlgItem(IDC_EDIT_LOGINACCOUNTNO).SetWindowText(L"");
	this->GetDlgItem(IDC_EDIT_TRADEACCOUNT).SetWindowText(L"");
#endif
#ifdef ZM_TDXSERVER_DEMO
	this->GetDlgItem(IDC_EDIT_TRADESERVERADDR).SetWindowText(L"mock.tdx.com.cn");
	this->GetDlgItem(IDC_EDIT_TRADESERVERPORT).SetWindowText(L"7708");
	this->GetDlgItem(IDC_EDIT_YYBID).SetWindowText(L"9000");
	this->GetDlgItem(IDC_EDIT_LOGINACCOUNTNO).SetWindowText(L"1852983998@qq.com");
	this->GetDlgItem(IDC_EDIT_TRADEACCOUNT).SetWindowText(L"001001001020115");
#endif

	this->GetDlgItem(IDC_EDIT_TRADEPASSWORD).SetWindowText(L"");
	this->GetDlgItem(IDC_EDIT_COMMPASSWORD).SetWindowText(L"");
	this->GetDlgItem(IDC_EDIT_STOCKCODE).SetWindowText(L"000001");

	return TRUE;
}

LRESULT CMainDlg::OnInitReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	IDispatch* piTrade = (IDispatch* )lParam;
	IStockTradePtr spiTrade(piTrade);
	if(NULL == spiTrade)
		return 0;/// 错误交易接口
	if(wParam)
	{
#ifdef _DEBUG
		/// 事件通知中尽量避免弹窗MessageBox等阻塞操作，弹窗等仅限于调试程序方便
		if(m_spiTrade[0] == spiTrade)
		{
			this->MessageBox(L"对象1初始化接口成功！");

		}
		else if(m_spiTrade[1] == spiTrade)
			this->MessageBox(L"对象2初始化接口成功！");
#endif
	}
	else
	{
		/// 连接失败，弹出错误描述
		CComBSTR bstrErrInfo;
		spiTrade->get_LastErrDesc(&bstrErrInfo);
#ifdef _DEBUG
		/// 事件通知中尽量避免弹窗MessageBox等阻塞操作，弹窗等仅限于调试程序方便
		this->MessageBox(bstrErrInfo.m_str);
#endif
		bstrErrInfo.Empty();
	}

	return 0;
}

LRESULT CMainDlg::OnLoginReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	IDispatch* piTrade = (IDispatch* )lParam;
	IStockTradePtr spiTrade(piTrade);
	if(NULL == spiTrade)
		return 0;/// 错误交易接口
	if(wParam)
	{
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		HRESULT hRet = spiTrade->get_ConnectValid(&bConnectValid);
		USHORT nTradeID = 0;
		hRet = spiTrade->get_CurTradeID(&nTradeID);
		/// 事件通知中尽量避免弹窗MessageBox等阻塞操作，弹窗等仅限于调试程序方便
		this->MessageBox(L"异步登录服务器成功！下面开始获取股东代码信息！");
		ITradeRecordPtr spiRecord = NULL;
		/// 获得股东账号信息
		spiTrade->get_ShareHolderCode(&spiRecord);
		if(NULL != spiRecord)
		{
			/// 获取JSON编码的数据内容
			CComBSTR bstrJsonVal;
			spiRecord->GetJsonString(&bstrJsonVal);
			this->MessageBox(bstrJsonVal.m_str);
			bstrJsonVal.Empty();
			/// 股东代码对象，底层会缓存数据备用，所以请尽量不要调用Clear清空，避免影响底层功能
//			spiRecord->Clear();
			spiRecord = NULL;
		}
#ifdef ZM_TDXSERVER_SYNC
		/// 同步请求某只股票的实时5档行情
		spiRecord = NULL;
		this->MessageBox(L"下面开始获取指定股票的实时5档行情！");
		spiTrade->GetStockQuote(nTradeID,CComBSTR(L"000001"),&spiRecord);
		ATLASSERT(spiRecord);
		ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
		if(NULL != spiRecord)
		{
			hRet = spiRecord->get_FieldCount(&nFieldCount);
			hRet = spiRecord->get_RecordCount(&nRecordCount);
			CComBSTR bstrJsonVal;
			spiRecord->GetJsonString(&bstrJsonVal);
			this->MessageBox(bstrJsonVal.m_str);
			bstrJsonVal.Empty();
		}
#else
		/// 异步请求多只股票的实时5档行情，可指定优先级和返回的请求序列号。nReqID会在事件回调中传回
		ULONG nReqID = 0;
		spiTrade->GetStockQuotes(nTradeID,CComBSTR(L"000001;000002;600001"),RUNPRITYPE_ABOVE_NORMAL,&nReqID);
#endif
	}
	else
	{
		/// 连接失败，弹出错误描述
		CComBSTR bstrErrInfo;
		spiTrade->get_LastErrDesc(&bstrErrInfo);
#ifdef _DEBUG
		/// 事件通知中尽量避免弹窗MessageBox等阻塞操作，弹窗等仅限于调试程序方便
		this->MessageBox(bstrErrInfo.m_str);
#endif
		bstrErrInfo.Empty();
	}

	return 0;
}

LRESULT CMainDlg::OnOrderOK(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiTradeClientEvent[lParam] || NULL == m_spiTrade[lParam])
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiTradeClientEvent[lParam]->GetReqData();
	if(NULL == spiRecord)
		return 0;
	ULONG nRecordCount = 0,nFieldCount = 0;
	spiRecord->get_FieldCount(&nFieldCount);
	spiRecord->get_RecordCount(&nRecordCount);
	if(!nFieldCount || !nRecordCount)
		return 0;/// 异常数据
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();
	/// 获取到委托序号，正常业务流程需要记录请求ReqID和成功回报OrderID的对应关系，便于后面进行撤单
	LONG nOrderID = 0;
	spiRecord->GetValueInt(0,0,&nOrderID);
	spiRecord->Clear();
	spiRecord = NULL;
	/// 执行取消委托，需要传入市场类型，由委托成功时通知返回，也可以自己计算，上海交易所2，深圳交易所为1
	m_spiTrade[lParam]->CancelOrder((EZMExchangeType)wParam,nOrderID,&spiRecord);
	return 0;
}

LRESULT CMainDlg::OnOrderSuccess(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiTradeClientEvent[lParam] || NULL == m_spiTrade[lParam])
		return 0;/// 对象已经释放
	/// 委托成功通知,编号在wParam
	return 0;
}

LRESULT CMainDlg::OnOrderError(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiTradeClientEvent[lParam] || NULL == m_spiTrade[lParam])
		return 0;/// 对象已经释放
	return 0;
}

LRESULT CMainDlg::OnStockQuote(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiTradeClientEvent[lParam])
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiTradeClientEvent[lParam]->GetReqData();
	if(NULL == spiRecord)
		return 0;
	/// 根据接口方法遍历数据
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	/// 获取JSON编码的数据内容
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType;
		spiRecord->GetDataType(nIndex,&eDataType);
		/// 输出返回记录的字段名称
		hRet = spiRecord->GetFieldName(nIndex,&bstrName);
		bstrName.Empty();
	}
	///  输出所有记录
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		for(ULONG n = 0;n < nFieldCount;n++)
		{
			/// 获取每个字段值
			CComVariant varVal;
			hRet = spiRecord->GetValue(nIndex,n,&varVal);
			varVal.Clear();
		}
	}
	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMainDlg::OnChangeServer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{

	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	ReleaseCom();

	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ReleaseCom();
	EndDialog(wID);
	return 0;
}

LONG StrToNum(const ATL::CString& strTemp)
{
	LONG nValue = 0;
	if(strTemp.IsEmpty())
		return nValue;
	BOOL bLoss = FALSE;
	if(strTemp.GetAt(0) == '-')
		bLoss = TRUE;
	int j = 0;
	if(bLoss)
		j++;
	for (; j < strTemp.GetLength(); j++)
	{
		TCHAR c = strTemp[j];
		if (c <= '9' && c >= '0')
		{
			nValue *= 10;
			nValue = (nValue + (c - '0')) ; 
		}
	}
	if(bLoss)
		return -nValue;
	return nValue;
}

void CMainDlg::ReleaseCom()
{
	/// 释放COM组件对象
	if(NULL != m_spiTrade[0])
	{
		m_spiTrade[0]->LogOut(0);
		/// 断开事件连接
		UnAdviseTradeClient(0);
		m_spiTrade[0] = NULL;
	}

	if(NULL != m_spiTrade[1])
	{
		m_spiTrade[1]->LogOut(0);
		/// 断开事件连接
		UnAdviseTradeClient(1);
		m_spiTrade[1] = NULL;
	}
}

LRESULT CMainDlg::OnBnClickedUnInit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL != m_spiTrade[0])
	{
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		m_spiTrade[0]->get_ConnectValid(&bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			this->MessageBox(L"还没有建立与交易服务器的连接！");
			return 0;
		}
		m_spiTrade[0]->LogOut(0);
	}

	this->MessageBox(L"断开通讯成功！");
	return 0;
}

LRESULT CMainDlg::OnBnClickedInit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL == m_spiTrade)
	{
		this->MessageBox(L"交易COM组件对象还没有创建成功！");
		return 0;
	}
	/// 获取初始化参数
	CComBSTR bstrServerAddr,bstrServerPort,bstrYybID;
	this->GetDlgItem(IDC_EDIT_TRADESERVERADDR).GetWindowText(&bstrServerAddr);
	this->GetDlgItem(IDC_EDIT_TRADESERVERPORT).GetWindowText(&bstrServerPort);
	this->GetDlgItem(IDC_EDIT_YYBID).GetWindowText(&bstrYybID);
	ATLASSERT(bstrServerAddr.Length());
	ATLASSERT(bstrServerPort.Length());
	ATLASSERT(bstrYybID.Length());
	CComBSTR bstrLoginID,bstrTradeAccount;
	this->GetDlgItem(IDC_EDIT_LOGINACCOUNTNO).GetWindowText(&bstrLoginID);
	this->GetDlgItem(IDC_EDIT_TRADEACCOUNT).GetWindowText(&bstrTradeAccount);
	ATLASSERT(bstrLoginID.Length());
	CComBSTR bstrTradePassword,bstrCommPassword;
	this->GetDlgItem(IDC_EDIT_TRADEPASSWORD).GetWindowText(&bstrTradePassword);
	this->GetDlgItem(IDC_EDIT_COMMPASSWORD).GetWindowText(&bstrCommPassword);
	if(!bstrTradePassword.Length())
	{
		this->MessageBox(L"还没有设置交易密码，无法登录服务器！");
		return 0;
	}

#ifdef ZM_TDXSERVER_DEMO
	m_spiTrade[0]->put_BrokerType(BROKERTYPE_MNCS);
#else
#ifdef ZM_BROKER_HFZQ
	m_spiTrade[0]->put_BrokerType(BROKERTYPE_HFZQ);
#endif
#ifdef ZM_BROKER_ZXZQ
	m_spiTrade[0]->put_BrokerType(BROKERTYPE_ZXZQ);
#endif
#endif

#ifdef ZM_TDXSERVER_DEMO
	m_spiTrade[0]->put_AccountType(LOGINIACCOUNTTYPE_MNCS);
#endif
#ifdef ZM_BROKER_HFZQ
	m_spiTrade[0]->put_AccountType(LOGINIACCOUNTTYPE_CUSTOMER);
#endif
#ifdef ZM_BROKER_ZXZQ
	m_spiTrade[0]->put_AccountType(LOGINIACCOUNTTYPE_CAPITAL);
#endif

#ifndef ZM_ADV_VERSION
	/// 设置交易服务器地址
	m_spiTrade[0]->put_CurServerHost(bstrServerAddr);
	/// 设置交易服务器端口
	m_spiTrade[0]->put_CurServerPort((USHORT)StrToNum(bstrServerPort.m_str));
#else

#ifdef ZM_TDXSERVER_DEMO
	/// 设置交易服务器地址
	m_spiTrade[0]->put_CurServerHost(bstrServerAddr);
	/// 设置交易服务器端口
	m_spiTrade[0]->put_CurServerPort((USHORT)StrToNum(bstrServerPort.m_str));
#else
	/// 高级版，通过JSON传入多个服务器地址，底层支持同时多个连接，一旦发现某个服务器操作故障，可以随时切换使用
	CString strJsonServer;
	/// 主机IP，端口，main表示主要使用使用，其他为备用，设置5个服务器地址，第一个主用
	strJsonServer = L"[{\"host\":\"202.108.253.186\",\"port\":7708,\"main\":1},{\"host\":\"202.108.253.141\",\"port\":7708,\"main\":0},{\"host\":\"27.221.115.134\",\"port\":7708,\"main\":0},{\"host\":\"124.160.88.188\",\"port\":7708,\"main\":0},{\"host\":\"124.160.88.188\",\"port\":7708,\"main\":0}]";
	m_spiTrade[0]->put_ServerConfig(CComBSTR(strJsonServer));
#endif
#endif

	/// 设置营业部ID
	m_spiTrade[0]->put_DepartmentID((USHORT)StrToNum(bstrYybID.m_str));
	/// 设置登录账户
	m_spiTrade[0]->put_LoginID(bstrLoginID);
	if(bstrTradeAccount.Length())
		m_spiTrade[0]->put_TradeAccount(bstrTradeAccount);/// 如果用资金账号登录，此值一般和登录账号一样，可以不设置
	/// 设置交易密码
	m_spiTrade[0]->put_TradePassword(bstrTradePassword);
	/// 设置通信密码
	if(bstrCommPassword.Length())
		m_spiTrade[0]->put_CommPassword(bstrCommPassword);/// 可以不设置，部分券商需要设置成和交易密码一样的

	VARIANT_BOOL bRetFlag = VARIANT_FALSE;
#ifndef ZM_TDXSERVER_SYNC
	///  异步执行登录交易服务器，登录结果通过事件通知获得
	HRESULT hRet = m_spiTrade[0]->LogIn(VARIANT_TRUE,&bRetFlag);
#else
	///  同步执行登录交易服务器，直到返回结果
	HRESULT hRet = m_spiTrade[0]->LogIn(VARIANT_FALSE,&bRetFlag);
	if(VARIANT_FALSE == bRetFlag)
	{
		/// 获取错误信息
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrType(&nErrCode);
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return 0;
	}
	this->MessageBox(L"同步登录交易服务器成功！");
#endif
	return 1;
}

ITradeRecordPtr CMainDlg::GetCategoryData(EZMStockQueryType eCategoryType)
{
	ITradeRecordPtr spiRecord = NULL;
	USHORT nTradeID = 0;
	HRESULT hRet = m_spiTrade[0]->get_CurTradeID(&nTradeID);
	if(!nTradeID)
	{
		this->MessageBox(L"请先初始化登录！");
		return spiRecord;
	}
	if(nTradeID)
	{
		/// 一般是初始化成功后使用一段时间，在提交交易和查询请求前、或这些请求失败时才检查下连接是否正常
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		hRet = m_spiTrade[0]->get_ConnectValid(&bConnectValid);
		ATLASSERT(VARIANT_TRUE == bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			VARIANT_BOOL bRetFlag = VARIANT_FALSE;
			/// 重新执行同步登录请求，原来设置的相关参数会自动再次使用
			hRet = m_spiTrade[0]->LogIn(VARIANT_FALSE,&bRetFlag);
		}
	}
	if(FAILED(hRet))
		return spiRecord;/// 通信不正常
	hRet = m_spiTrade[0]->QueryTradeData(nTradeID,eCategoryType,&spiRecord);
	if(NULL == spiRecord)
	{
		/// 获取错误信息
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrType(&nErrCode);
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return spiRecord;
	}
	ATLASSERT(spiRecord);
	return spiRecord;
}

LRESULT CMainDlg::OnBnClickedGetTradeData(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	int nIndex = m_wndTradeList.GetCurSel();
	DWORD dwType = (DWORD )m_wndTradeList.GetItemData(nIndex);
	if(NULL == m_spiTrade)
		return 0;
	ITradeRecordPtr spiRecord = GetCategoryData((EZMStockQueryType)dwType);
	if(NULL != spiRecord)
	{
		ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
		HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
		hRet = spiRecord->get_RecordCount(&nRecordCount);
		CComBSTR bstrJsonVal;
		spiRecord->GetJsonString(&bstrJsonVal);
		this->MessageBox(bstrJsonVal.m_str);
		bstrJsonVal.Empty();

		CComBSTR bstrName;
		for(nIndex = 0;nIndex < nFieldCount;nIndex++)
		{
			/// 输出返回记录的字段类型
			EZMDataType eDataType;
			spiRecord->GetDataType(nIndex,&eDataType);
			/// 输出返回记录的字段名称
			hRet = spiRecord->GetFieldName(nIndex,&bstrName);
			bstrName.Empty();
		}
		///  输出所有记录
		for(nIndex = 0;nIndex < nRecordCount;nIndex++)
		{
			for(ULONG n = 0;n < nFieldCount;n++)
			{
				/// 获取每个字段值
				CComVariant varVal;
				hRet = spiRecord->GetValue(nIndex,n,&varVal);
				varVal.Clear();
			}
		}
		spiRecord->Clear();
		spiRecord = NULL;
	}
	return 0;
}

LRESULT CMainDlg::OnBnClickedGetFiveMarket(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_STOCKCODE).GetWindowText(&bstrStockCode);
	ATLASSERT(bstrStockCode.Length());
	ITradeRecordPtr spiRecord = GetStockMarket(bstrStockCode.m_str);
	if(NULL == spiRecord)
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType = ZMDATATYPE_UNKNOW;
		spiRecord->GetDataType(nIndex,&eDataType);
		/// 输出返回记录的字段名称
		hRet = spiRecord->GetFieldName(nIndex,&bstrName);
		bstrName.Empty();
	}
	///  输出所有记录
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		for(ULONG n = 0;n < nFieldCount;n++)
		{
			/// 获取每个字段值
			CComVariant varVal;
			hRet = spiRecord->GetValue(nIndex,n,&varVal);
			varVal.Clear();
		}
	}
	spiRecord->Clear();
	spiRecord = NULL;
	return 0;
}

ITradeRecordPtr CMainDlg::GetHisData(EZMHisOrderType eCategoryType)
{
	ITradeRecordPtr spiRecord = NULL;
	USHORT nTradeID = 0;
	HRESULT hRet = m_spiTrade[0]->get_CurTradeID(&nTradeID);
	if(!nTradeID)
	{
		this->MessageBox(L"请先初始化登录！");
		return spiRecord;
	}
	if(nTradeID)
	{
		/// 一般是初始化成功后使用一段时间，在提交交易和查询请求前、或这些请求失败时才检查下连接是否正常
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		hRet = m_spiTrade[0]->get_ConnectValid(&bConnectValid);
		ATLASSERT(VARIANT_TRUE == bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			VARIANT_BOOL bRetFlag = VARIANT_FALSE;
			/// 重新执行同步登录请求，原来设置的相关参数会自动再次使用
			hRet = m_spiTrade[0]->LogIn(VARIANT_FALSE,&bRetFlag);
		}
	}
	if(FAILED(hRet))
		return spiRecord;/// 通信不正常
	CComBSTR bstrStartDay(L"20170415"),bstrEndDay(L"20170421");
	hRet = m_spiTrade[0]->QueryHisTradeData(nTradeID,eCategoryType,bstrStartDay,bstrEndDay,&spiRecord);
	if(NULL == spiRecord)
	{
		/// 获取错误信息
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrType(&nErrCode);
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return spiRecord;
	}
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType = ZMDATATYPE_UNKNOW;
		spiRecord->GetDataType(nIndex,&eDataType);
		/// 输出返回记录的字段名称
		hRet = spiRecord->GetFieldName(nIndex,&bstrName);
		bstrName.Empty();
	}
	///  输出所有记录
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		for(ULONG n = 0;n < nFieldCount;n++)
		{
			/// 获取每个字段值
			CComVariant varVal;
			hRet = spiRecord->GetValue(nIndex,n,&varVal);
			varVal.Clear();
		}
	}
	spiRecord->Clear();
	spiRecord = NULL;
	return spiRecord;
}

LRESULT CMainDlg::OnBnClickedGetHisData(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	DWORD dwType = (DWORD )m_wndHistoryList.GetItemData(m_wndHistoryList.GetCurSel());
	if(NULL == m_spiTrade)
		return 0;
	ITradeRecordPtr spiRecord = GetHisData((EZMHisOrderType)dwType);
	if(NULL == spiRecord)
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType = ZMDATATYPE_UNKNOW;
		spiRecord->GetDataType(nIndex,&eDataType);
		/// 输出返回记录的字段名称
		hRet = spiRecord->GetFieldName(nIndex,&bstrName);
		bstrName.Empty();
	}
	///  输出所有记录
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		for(ULONG n = 0;n < nFieldCount;n++)
		{
			/// 获取每个字段值
			CComVariant varVal;
			hRet = spiRecord->GetValue(nIndex,n,&varVal);
			varVal.Clear();
		}
	}
	spiRecord->Clear();
	spiRecord = NULL;
	return 0;
}

ITradeRecordPtr CMainDlg::GetStockMarket(const CString& strStockCode)
{
	ITradeRecordPtr spiRecord = NULL;
	if(NULL == m_spiTrade)
	{
		this->MessageBox(L"组件还未创建成功！");
		return spiRecord;
	}
	USHORT nTradeID = 0;
	HRESULT hRet = m_spiTrade[0]->get_CurTradeID(&nTradeID);
	if(!nTradeID)
	{
		this->MessageBox(L"请先初始化登录！");
		return spiRecord;
	}
	if(nTradeID)
	{
		/// 一般是初始化成功后使用一段时间，在提交交易和查询请求前、或这些请求失败时才检查下连接是否正常
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		hRet = m_spiTrade[0]->get_ConnectValid(&bConnectValid);
		ATLASSERT(VARIANT_TRUE == bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			VARIANT_BOOL bRetFlag = VARIANT_FALSE;
			/// 重新执行同步登录请求，原来设置的相关参数会自动再次使用
			hRet = m_spiTrade[0]->LogIn(VARIANT_FALSE,&bRetFlag);
		}
	}
	if(FAILED(hRet))
	{
		this->MessageBox(L"通信不正常！");
		return spiRecord;/// 通信不正常
	}
	hRet = m_spiTrade[0]->GetStockQuote(nTradeID,CComBSTR(strStockCode),&spiRecord);
	if(NULL == spiRecord)
	{
		/// 获取错误信息
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrType(&nErrCode);
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return spiRecord;
	}
	ATLASSERT(spiRecord);
	return spiRecord;
}

LRESULT CMainDlg::OnBnClickedBuy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_STOCKCODE).GetWindowText(&bstrStockCode);
	ATLASSERT(bstrStockCode.Length());
	ITradeRecordPtr spiRecord = GetStockMarket(bstrStockCode.m_str);
	if(NULL == spiRecord)
		return 0;
	/// 所有接口需要处理的异常，如果COM组件服务进程崩溃，在请求接口功能的时候，会返回错误码0x800706BA，意思是“RPC 服务器不可用”
	/// 需要释放当前使用的交易接口对象后重新创建 HRESULT hRet = m_spiTrade.CreateInstance(__uuidof(Trade)) 后再自行连接

	CComVariant varVal;
	/// 取卖一价格主动买入
	HRESULT hRet(E_FAIL);
	/// 根据序号获取可能存在的问题：需要适配券商，券商不同，可能返回的字段顺序不一样
//	hRet = spiRecord->GetValue(0,16,&varVal);
	/// 或者用字段标题获取
	hRet = spiRecord->GetValueByName(0,CComBSTR(L"买五价"),&varVal);
	ULONG nReqID1 = 0;/// 返回唯一标识的委托ID，组件会以事件方式通知结果，如果您需要跟踪每个委托的结果通知，需要记录下来，和事件通知的ID进行匹配
	m_spiTrade[0]->AddOrder(STOCKORDERTYPE_BUY,ORDERPRICETYPE_LIMIT,bstrStockCode,varVal.fltVal,500,&nReqID1);
	varVal.Clear();
	/// 取当前价买入
//	hRet = spiRecord->GetValue(0,5,&varVal);
	hRet = spiRecord->GetValueByName(0,CComBSTR(L"买四价"),&varVal);
	ULONG nReqID2 = 0;/// 返回唯一标识的委托ID，组件会以事件方式通知结果
//	hRet = m_spiTrade[0]->AddOrder(STOCKORDERTYPE_BUY,ORDERPRICETYPE_LIMIT,bstrStockCode,varVal.fltVal,500,&nReqID2);
	bstrStockCode.Empty();

	/// 提交委托，第一个参数如果为VARIANT_TRUE,底层会自动检查当前通信状态，不正常的时候会自动登录一次再提交委托
	VARIANT_BOOL bCommitRet = VARIANT_FALSE;
	/// 可设置提交的优先级
	USHORT nTradeID = 0;
	m_spiTrade[0]->get_CurTradeID(&nTradeID);
	hRet = m_spiTrade[0]->CommitOrder(nTradeID,VARIANT_FALSE,RUNPRITYPE_NORMAL,&bCommitRet);
	spiRecord->Clear();
	spiRecord = NULL;
	if(VARIANT_FALSE == bCommitRet)
	{
		/// 获取错误信息
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrType(&nErrCode);
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return 0;
	}
	if(NULL != spiRecord)
	{
		/// 获得委托回报
		spiRecord->Clear();
		spiRecord = NULL;
	}
	return 0;
}

LRESULT CMainDlg::OnBnClickedSell(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	ITradeRecordPtr spiRecord = NULL;
	USHORT nTradeID = 0;
	HRESULT hRet = m_spiTrade[0]->get_CurTradeID(&nTradeID);
	if(!nTradeID)
	{
		this->MessageBox(L"请先初始化登录！");
		return spiRecord;
	}
	if(nTradeID)
	{
		/// 一般是初始化成功后使用一段时间，在提交交易和查询请求前、或这些请求失败时才检查下连接是否正常
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		hRet = m_spiTrade[0]->get_ConnectValid(&bConnectValid);
		ATLASSERT(VARIANT_TRUE == bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			VARIANT_BOOL bRetFlag = VARIANT_FALSE;
			/// 重新执行同步登录请求，原来设置的相关参数会自动再次使用
			hRet = m_spiTrade[0]->LogIn(VARIANT_FALSE,&bRetFlag);
		}
	}
	if(FAILED(hRet))
		return spiRecord;/// 通信不正常
	/// 请求股票列表
	hRet = m_spiTrade[0]->QueryTradeData(nTradeID,STOCKQUERYTYPE_STOCK,&spiRecord);
	if(NULL == spiRecord)
	{
		/// 获取错误信息
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrType(&nErrCode);
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return spiRecord;
	}
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	if(!nFieldCount || !nRecordCount)
	{
		spiRecord = NULL;
		return 0;
	}
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	/// 查找可卖出的股票
	CComVariant varVal;
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		spiRecord->GetValueByName(nIndex,CComBSTR(L"可用股份"),&varVal);
		varVal.ChangeType(VT_I2);
		if(!varVal.iVal)
			continue;/// 无可用，无法卖出的
		int nSellCount = varVal.iVal;
		varVal.Clear();
		spiRecord->GetValueByName(nIndex,CComBSTR(L"证券名称"),&varVal);
		CString strStockName(varVal.bstrVal);
		varVal.Clear();
		spiRecord->GetValueByName(nIndex,CComBSTR(L"证券代码"),&varVal);
		CString strStockCode(varVal.bstrVal);
		varVal.Clear();
		spiRecord->GetValueByName(nIndex,CComBSTR(L"股东代码"),&varVal);
		CString strHolderCode(varVal.bstrVal);
		varVal.Clear();

		/// 获取当前行情价格
		ITradeRecordPtr spiQuote = NULL;
		m_spiTrade[0]->GetStockQuote(nTradeID,CComBSTR(strStockCode),&spiQuote);
		if(NULL == spiQuote)
			continue;
		ULONG nQuoteCount = 0;
		spiQuote->get_RecordCount(&nQuoteCount);
		if(!nQuoteCount)
		{
			/// 没有获取到当前行情
			spiQuote = NULL;
			continue;
		}
		CComBSTR bstrJsonVal;
		spiQuote->GetJsonString(&bstrJsonVal);
		this->MessageBox(bstrJsonVal.m_str);
		bstrJsonVal.Empty();

		/// 挂卖5价卖出
		spiQuote->GetValueByName(0,CComBSTR(L"卖五价"),&varVal);
		varVal.ChangeType(VT_R4);
		spiQuote = NULL;
		float fSell = varVal.fltVal;
		varVal.Clear();
		/// 限价卖出所有可用股票
#ifndef ZM_TDXSERVER_SYNC
		/// 异步提交，在这儿要记录返回的请求ID，便于和事件通知里的委托回报相对应，从而本次知道实际委托结果
		ULONG nReqID = 0;
		m_spiTrade[0]->AddOrder(STOCKORDERTYPE_SALE,ORDERPRICETYPE_LIMIT,\
			CComBSTR(strStockCode),fSell,nSellCount,&nReqID);
#else
		/// 同步提交
		EZMExchangeType eExchangeType = EXCHANGETYPE_UNKNOWN;
		ITradeRecordPtr spiSell = NULL;
		m_spiTrade[0]->SyncCommitOrder(VARIANT_TRUE,STOCKORDERTYPE_SALE,ORDERPRICETYPE_LIMIT,\
			CComBSTR(strStockCode),fSell,nSellCount,&eExchangeType,&spiSell);
		if(NULL == spiSell)
		{
			/// 提交失败，获取错误描述
			CComBSTR bstrErrInfo;
			m_spiTrade[0]->get_LastErrDesc(&bstrErrInfo);
			continue;
		}
		ULONG nSellRecord = 0;
		spiSell->get_RecordCount(&nSellRecord);
		if(nSellRecord)
		{
			CComBSTR bstrJsonVal;
			spiSell->GetJsonString(&bstrJsonVal);
			this->MessageBox(bstrJsonVal.m_str);
			bstrJsonVal.Empty();

			/// 获得交易服务器上返回的委托回报编号ID，通过此ID可以进行撤单操作
			spiSell->GetValue(0,0,&varVal);
			varVal.ChangeType(VT_I4);
			ITradeRecordPtr spiCancle = NULL;
			m_spiTrade[0]->CancelOrder(eExchangeType,varVal.lVal,&spiCancle);
			/// 通过提取spiCancle里的返回值得知撤单结果
			spiCancle = NULL;
			varVal.Clear();
		}
		spiSell = NULL;
#endif
	}
	spiRecord->Clear();
	spiRecord = NULL;
#ifndef ZM_TDXSERVER_SYNC
	VARIANT_BOOL bCommitRet = VARIANT_FALSE;
	m_spiTrade[0]->CommitOrder(nTradeID,VARIANT_TRUE,RUNPRITYPE_NORMAL,&bCommitRet);
#endif
	return 0;
}