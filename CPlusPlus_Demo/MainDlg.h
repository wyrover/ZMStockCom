// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "StockComEvent.h"

class CMainDlg : public CAxDialogImpl<CMainDlg>
{
protected:

	/// StockTrade服务接口数组，需要几个就声明几个，也可以用列表来管理
	IStockTradePtr		m_spiTrade[2];

	/// 交易查询类型
	CComboBox		m_wndTradeList;
	/// 历史数据类型
	CComboBox		m_wndHistoryList;

	/// 交易接口事件COOKIE数组
	DWORD							m_nTradeEventCookie[2];
	/// 交易接口事件对象数组
	CComObject<CStockComEvent>*		m_spiTradeClientEvent[2];

	/// 建立事件通知
	void AdviseTradeClient(int nIndex);
	/// 断开事件通知
	void UnAdviseTradeClient(int nIndex);

	/// 释放组件
	void ReleaseCom();

	///获得交易数据
	ITradeRecordPtr GetCategoryData(EZMStockQueryType eCategoryType);
	///获得历史委托数据
	ITradeRecordPtr GetHisData(EZMHisOrderType eCategoryType);

	/// 获得指定股票当前实时行情
	ITradeRecordPtr GetStockMarket(const CString& strStockCode);

public:
	enum { IDD = IDD_MAINDLG };

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

		MESSAGE_HANDLER(WM_TRADEEVENT_INITRETURN, OnInitReturn)
		MESSAGE_HANDLER(WM_TRADEEVENT_LOGINRETURN, OnLoginReturn)
		MESSAGE_HANDLER(WM_TRADEEVENT_ORDEROK, OnOrderOK)
		MESSAGE_HANDLER(WM_TRADEEVENT_ORDERERROR, OnOrderError)
		MESSAGE_HANDLER(WM_TRADEEVENT_STOCKQUOTE, OnStockQuote)
		MESSAGE_HANDLER(WM_TRADEEVENT_CHANGESERVER, OnChangeServer)
		MESSAGE_HANDLER(WM_TRADEEVENT_ORDERSUCCESS, OnOrderSuccess)

		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)

		COMMAND_HANDLER(IDC_BUTTON_INIT, BN_CLICKED, OnBnClickedInit)
		COMMAND_HANDLER(IDC_BUTTON_UNINIT, BN_CLICKED, OnBnClickedUnInit)
		COMMAND_HANDLER(IDC_BUTTON_GETTRADE, BN_CLICKED, OnBnClickedGetTradeData)
		COMMAND_HANDLER(IDC_BUTTON_FIVEMARKET, BN_CLICKED, OnBnClickedGetFiveMarket)
		COMMAND_HANDLER(IDC_BUTTON_HISDATA, BN_CLICKED, OnBnClickedGetHisData)
		COMMAND_HANDLER(IDC_BUTTON_BUY, BN_CLICKED, OnBnClickedBuy)
		COMMAND_HANDLER(IDC_BUTTON_SELL, BN_CLICKED, OnBnClickedSell)

	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnInitReturn(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLoginReturn(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOrderOK(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOrderError(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnStockQuote(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnChangeServer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOrderSuccess(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnBnClickedInit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedGetFiveMarket(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedGetTradeData(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedGetHisData(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBuy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedSell(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnBnClickedUnInit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
