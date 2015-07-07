
// DemoClientDlg.h : ͷ�ļ�
//

#pragma once
#include "DemoUse.h"

// CDemoClientDlg �Ի���
class CDemoClientDlg : public CDialogEx
{
// ����
public:
	CDemoClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DEMOCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


	
public:
	afx_msg void OnDestroy();

	//! Check here!
	//!��ر���
	ff::EDefaultErrorHandler * m_pEH;
	IPCSignal_t * m_pStartSig; //!Start�ź�
	IPCSignal_t * m_pEndSig; //End�ź�
	IPCSignal_t * m_pReadSig; //���ݿɶ��ź�
	IPCShMem_t * m_pShMem; //�����ڴ�
	MSignalHandler_t * m_pSigHandler; //�źŴ�����

	int m_iMyData;

	afx_msg void OnBnClickedLanuchBtn();
};

DWORD start_fill_sh_mem(LPVOID lpParam); //Start�źŵĻص�����
DWORD end_fill_sh_mem(LPVOID lpParam); //End�źŵĻص�����
