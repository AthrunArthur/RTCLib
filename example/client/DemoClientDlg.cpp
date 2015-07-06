
// DemoClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DemoClient.h"
#include "DemoClientDlg.h"
#include "afxdialogex.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDemoClientDlg 对话框



CDemoClientDlg::CDemoClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDemoClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDemoClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDemoClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_LANUCH_BTN, &CDemoClientDlg::OnBnClickedLanuchBtn)
END_MESSAGE_MAP()


// CDemoClientDlg 消息处理程序

BOOL CDemoClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_pEH = new ff::EDefaultErrorHandler();
	m_pStartSig = new IPCSignal_t(m_pEH);
	m_pStartSig->initialize(START_SEM_NAME, 0);
	m_pEndSig = new IPCSignal_t(m_pEH);
	m_pEndSig->initialize(END_SEM_NAME, 0);
	m_pReadSig = new IPCSignal_t(m_pEH);
	m_pReadSig->initialize(READ_SEM_NAME, 0);
	m_pShMem = new IPCShMem_t(m_pEH);
	m_pShMem->initialize(SH_MEM_NAME, SH_MEM_SIZE);

	m_pSigHandler = new MSignalHandler_t(m_pEH);
	//! 指定信号send后的回调函数
	m_pSigHandler->handle_signale(m_pStartSig, MAX_BLOCK_TIME, start_fill_sh_mem, (LPVOID)(this));
	m_pSigHandler->handle_signale(m_pEndSig, MAX_BLOCK_TIME, end_fill_sh_mem, (LPVOID)(this));
	m_pSigHandler->start(); //!注意：此处需要手动启动

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDemoClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDemoClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDemoClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_pSigHandler->stop();  //!TODO, should have a button
	delete m_pSigHandler;
	delete m_pShMem;
	delete m_pStartSig;
	delete m_pEndSig;
	delete m_pReadSig;
	delete m_pEH;
}

DWORD end_fill_sh_mem(LPVOID lpParam)
{
	CDemoClientDlg * pDlg = (CDemoClientDlg *) (lpParam);
	//! recv the end signal;
	//write data here!
	IPCShMem_t * pShMem = pDlg->m_pShMem;
	std::stringstream ss;
	ss<<"here is my data: "<<pDlg->m_iMyData<<"\n";
	std::string s = ss.str();
	memcpy(pShMem->addr(), s.c_str(), s.size());

	//When the writing is done, send signal;
	pDlg->m_pReadSig->send_signal();
	return 0;
}

DWORD start_fill_sh_mem(LPVOID lpParam)
{
	CDemoClientDlg * pDlg = (CDemoClientDlg *) (lpParam);
	
	//! recv the end signal;
	//start collect data here!
	pDlg->m_iMyData ++;
	return 0;
}


void CDemoClientDlg::OnBnClickedLanuchBtn()
{
	// 启动Server程序
	m_iMyData = 0;
#define MAX_PATH_LEN 256
	TCHAR szPath[MAX_PATH_LEN];
	GetModuleFileName( NULL, szPath, MAX_PATH_LEN ); 
	CString path(szPath);
	path.Replace(TEXT("DemoClient"), TEXT("DemoServer"));

	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

	TCHAR pathmem[MAX_PATH_LEN];
	lstrcpy(pathmem, (LPCTSTR)path);

	if( !CreateProcess( NULL,   // No module name (use command line)
        pathmem,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return;
    }

    // Wait until child process exits.
    //WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    //CloseHandle( pi.hProcess );
    //CloseHandle( pi.hThread );

}
