
// DemoClientDlg.h : 头文件
//

#pragma once
#include "DemoUse.h"

// CDemoClientDlg 对话框
class CDemoClientDlg : public CDialogEx
{
// 构造
public:
	CDemoClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DEMOCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


	
public:
	afx_msg void OnDestroy();

	//! Check here!
	//!相关变量
	ff::EDefaultErrorHandler * m_pEH;
	IPCSignal_t * m_pStartSig; //!Start信号
	IPCSignal_t * m_pEndSig; //End信号
	IPCSignal_t * m_pReadSig; //数据可读信号
	IPCShMem_t * m_pShMem; //共享内存
	MSignalHandler_t * m_pSigHandler; //信号处理器

	int m_iMyData;

	afx_msg void OnBnClickedLanuchBtn();
};

DWORD start_fill_sh_mem(LPVOID lpParam); //Start信号的回调函数
DWORD end_fill_sh_mem(LPVOID lpParam); //End信号的回调函数
