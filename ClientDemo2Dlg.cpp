// ClientDemo2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientDemo2.h"
#include "ClientDemo2Dlg.h"
#include "Localpalyback.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientDemo2Dlg dialog

CClientDemo2Dlg::CClientDemo2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientDemo2Dlg::IDD, pParent)
{
	skinppLoadSkin(_T("bbq.ssk"));
	skinppSetSkinHwnd(this->m_hWnd);
	//{{AFX_DATA_INIT(CClientDemo2Dlg)
	m_VideoPath = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_nCurIndex = -1;
	m_nTotalWnd = 4;	//the default number of windows is 4
	memset(&m_devInfo, 0, sizeof(DEV_INFO));

}

void CClientDemo2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientDemo2Dlg)
	DDX_Control(pDX, IDC_CHECK_OSD, m_checkOsd);
	DDX_Control(pDX, IDC_COMBOWNDNUM, m_cmbWnd);
	DDX_Text(pDX, IDC_EDIT_URL, m_VideoPath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CClientDemo2Dlg, CDialog)
	//{{AFX_MSG_MAP(CClientDemo2Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CAPTURE, OnBtnCapture)
	ON_BN_CLICKED(IDC_BTN_DEVICE, OnBtnDevice)
	ON_BN_CLICKED(IDC_BTN_NET_ALARM, OnBtnNetAlarm)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBOWNDNUM, OnSelchangeCombowndnum)
	ON_BN_CLICKED(IDC_BTN_PLAYBACK, OnBtnPlayback)
	ON_BN_CLICKED(IDC_BUTTON_ASSURE, OnButtonAssure)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientDemo2Dlg message handlers

BOOL CClientDemo2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	_CWndCS(this);
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_devDLg.Create(CDeviceEditDlg::IDD, this);
	m_devDLg.ShowWindow(SW_SHOW);
	m_NetAlarmDLg.Create(CNetAlarmDlg::IDD, this);
	m_cmbWnd.InsertString(0,"4");
	m_cmbWnd.SetCurSel(0);
	
	//初始化16个显示窗口
	for ( int i = 0; i < MAXWNDNUM; i ++ )
	{
		m_videoWnd[i].Create(CVideoWnd::IDD, this);
		m_videoWnd[i].SetWndIndex(i);
	}

	InitSDK();//初始化网络库
	//通过计算显示客户端的界面大小
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	int width2 = height * 4 / 3;

	if (width2 < width)
	{
		MoveWindow(0, 0, width2, height - 30);
	}
	else if(width2 > width)
	{
		int height2 = width * 3 / 4;
		MoveWindow(0, 0, width, height2-30);
	}
	else
	{
		MoveWindow(0, 0, width, height);
	}
	ArrayWindow(4);
	SetActiveWnd(0);
	
	CRect rect;
	GetClientRect(&rect);
	//通过计算显示控件的坐标
	CRect btnRect;
	GetDlgItem(IDC_BTN_DEVICE)->GetClientRect(&btnRect);
	int nBtnWidth = btnRect.Width();
	int nBtnHeigth = btnRect.Height();
	
	CRect moveRect, tmpRect;
	moveRect.top = rect.bottom - (int)rect.Height()*0.198;
	moveRect.left = rect.right - (int)rect.Width()*0.234375;
	moveRect.bottom = moveRect.top + nBtnHeigth;
	moveRect.right = moveRect.left + nBtnWidth;
	GetDlgItem(IDC_BTN_DEVICE)->MoveWindow(moveRect);

	tmpRect = moveRect;
	tmpRect.left += nBtnWidth;
	tmpRect.right += nBtnWidth;
	GetDlgItem(IDC_BTN_NET_ALARM)->MoveWindow(tmpRect);

	moveRect.top += nBtnHeigth;
	moveRect.bottom += nBtnHeigth;
	GetDlgItem(IDC_BTN_PLAYBACK)->MoveWindow(moveRect);

	moveRect.top += nBtnHeigth;
	moveRect.bottom += nBtnHeigth;
//	GetDlgItem(IDC_BTN_COLOR)->MoveWindow(moveRect);

	tmpRect = moveRect;
	tmpRect.left += nBtnWidth;
	tmpRect.right += nBtnWidth;
//	GetDlgItem(IDC_BTN_PLAYBACK)->MoveWindow(tmpRect);

	moveRect.top += nBtnHeigth;
 	moveRect.bottom += nBtnHeigth;
//	GetDlgItem(IDC_BTN_DEV_KEYBOARD)->MoveWindow(moveRect);

	tmpRect = moveRect;
	tmpRect.left += nBtnWidth;
 	tmpRect.right += nBtnWidth;
//	GetDlgItem(IDC_BTN_NET_ALARM)->MoveWindow(tmpRect);


	CRect dlgRect;
	dlgRect.top = 0;
	dlgRect.left = tmpRect.left-btnRect.Width() ;
	dlgRect.bottom = rect.bottom - (int)rect.Height()*0.205;
	dlgRect.right = rect.right;

	m_devDLg.MoveWindow(dlgRect);
//	m_colorDLg.MoveWindow(dlgRect);
//	m_ptzDLg.MoveWindow(dlgRect);
//	m_kbDLg.MoveWindow(dlgRect);
	m_NetAlarmDLg.MoveWindow(dlgRect);

	moveRect.top = rect.bottom - 24;
	moveRect.bottom = rect.bottom;
	moveRect.left = 10;
	moveRect.right = moveRect.left + 60;
	GetDlgItem(IDC_STATIC_CMBWND)->MoveWindow(moveRect);

	moveRect.left = moveRect.right;
 	moveRect.right += nBtnWidth;
	GetDlgItem(IDC_COMBOWNDNUM)->MoveWindow(moveRect);

	moveRect.left = moveRect.right;
	moveRect.right += nBtnWidth;
	GetDlgItem(IDC_BTN_CAPTURE)->MoveWindow(moveRect);

	moveRect.left = moveRect.right;
	moveRect.right += nBtnWidth;
	GetDlgItem(IDC_BTN_SAVE)->MoveWindow(moveRect);
    
	moveRect.left = moveRect.right + 30 ;
	moveRect.right += 60;
	GetDlgItem(IDC_STATIC_URL)->MoveWindow(moveRect);

	moveRect.left = moveRect.right ;
	moveRect.right += 300;
	GetDlgItem(IDC_EDIT_URL)->MoveWindow(moveRect);
	
	moveRect.left = moveRect.right;
	moveRect.right += 30;
	GetDlgItem(IDC_BUTTON_ASSURE)->MoveWindow(moveRect);

	moveRect.left = moveRect.right;
	moveRect.right += 30;
	GetDlgItem(IDC_BUTTON_STOP)->MoveWindow(moveRect);
	

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientDemo2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDemo2Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientDemo2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CClientDemo2Dlg::ArrayWindow(WORD iNumber)//在视频显示区域内划分小的视频显示窗口
{
	m_nTotalWnd = iNumber;

	int i = 0;
	CRect Rect;
	GetClientRect(&Rect);
	WORD iWidth,iHeight;

	int nFullWidth=Rect.Width();
	int nFullHeight=Rect.Height();
	iWidth = (int) nFullWidth*0.75515625;
	iHeight = (int)nFullHeight*0.91;
	for (i = 0;i < MAXWNDNUM;i++)
	{
	   m_videoWnd[i].ShowWindow(SW_HIDE);
	}
	
	int nNull = 3;	

	switch(iNumber)
	{
	case 1:
		m_videoWnd[0].MoveWindow(3 + 0, 0, iWidth, iHeight);
		m_videoWnd[0].ShowWindow(SW_SHOW);
		break;
	case 4:
		for(i = 0; i < 2; i++)
		{
			m_videoWnd[i].MoveWindow( 3 + i * (iWidth / 2) + i * nNull , 0, (iWidth / 2), iHeight / 2);
			m_videoWnd[i].ShowWindow(SW_SHOW);
		}
		for(i = 2; i < 4; i++)
		{
			m_videoWnd[i].MoveWindow(3 + (i - 2) * (iWidth / 2) + (i - 2) * nNull, iHeight / 2 + nNull, (iWidth / 2), iHeight / 2);
			m_videoWnd[i].ShowWindow(SW_SHOW);
		}
		break;
	case 9:
		for (i=0;i<3;i++)
		{
			m_videoWnd[i].MoveWindow(3 + i * (iWidth / 3) + i * nNull, 0, (iWidth / 3), iHeight / 3);
			m_videoWnd[i].ShowWindow(SW_SHOW);
		}
		for (i=3;i<6;i++)
		{
			m_videoWnd[i].MoveWindow(3 +(i - 3) * (iWidth / 3) + (i - 3) * nNull, iHeight / 3 + nNull, (iWidth / 3), iHeight / 3);
			m_videoWnd[i].ShowWindow(SW_SHOW);
		}
		for (i=6;i<9;i++)
		{
			m_videoWnd[i].MoveWindow(3 + (i - 6) * (iWidth / 3) + (i - 6) * nNull, 2 * iHeight / 3 + 2 * nNull ,(iWidth / 3), iHeight / 3);
			m_videoWnd[i].ShowWindow(SW_SHOW);
		}
		break;
	case 16:
		for(i = 0; i < 4; i++)
		{
			m_videoWnd[i].MoveWindow(3 + i * (iWidth / 4) + (i) * nNull, 0, (iWidth / 4), iHeight / 4);
			m_videoWnd[i].ShowWindow(SW_SHOW);
		}
		for(i = 4; i < 8; i++)
		{
			m_videoWnd[i].MoveWindow(3 +(i - 4) * (iWidth / 4) + (i - 4) * nNull, iHeight / 4 + nNull, (iWidth / 4), iHeight / 4);
			m_videoWnd[i].ShowWindow(SW_SHOW);
		}
		for(i = 8; i < 12; i++)
		{
			m_videoWnd[i].MoveWindow(3 +(i - 8) * (iWidth / 4) + (i - 8) * nNull, iHeight / 2 + 2 * nNull, (iWidth / 4), iHeight / 4);
			m_videoWnd[i].ShowWindow(SW_SHOW);
		}
		for(i = 12; i < 16; i++)
		{
			m_videoWnd[i].MoveWindow(3 +(i - 12) * (iWidth / 4) + (i - 12) * nNull, 3 * iHeight / 4 + 3 * nNull, (iWidth / 4), iHeight / 4);
			m_videoWnd[i].ShowWindow(SW_SHOW);
		}
		break;
	default:
		break;
	}
}

int CClientDemo2Dlg::Connect(DEV_INFO *pDev, int nChannel, int nWndIndex /* = -1 */)
{
	int nRet = 0;
	SocketStyle type = UDPSOCKET;

	if ( !pDev )
	{
		return nRet;
	}

	//if device did not login,login first
	if ( pDev->lLoginID <= 0 )
	{
		H264_DVR_DEVICEINFO OutDev;
		int nError = 0;
		//设置尝试连接设备次数和等待时间
		H264_DVR_SetConnectTime(3000, 1);//设置尝试连接1次，等待时间3s
		long lLogin = H264_DVR_Login(pDev->szIpaddress, pDev->nPort, pDev->szUserName, pDev->szPsw, &OutDev, &nError, type );//登入设备
		if ( lLogin <= 0 )
		{
			CString sTemp;	
			int nErr = H264_DVR_GetLastError();//返回函数执行失败代码，当调用下面的接口失败时，可以用该函数获取失败的代码，具体错误代码参见
			if(nErr == H264_DVR_PASSWORD_NOT_VALID)
			{
				MessageBox(_CS("Error.PwdErr"));
			}
			else
			{
				MessageBox(_CS("Error.NotFound"));
			}

			return nRet;
		}

		pDev->lLoginID = lLogin;
		H264_DVR_SetupAlarmChan(lLogin);//开始对某个设备订阅消息,用来设置是否需要对设备消息回调，得到的消息从H264_DVR_SetDVRMessCallBack的设置值回调出来。
	}

	int nWnd = m_nCurIndex;
	if ( nWndIndex >= 0 )
	{
		nWnd = nWndIndex;
	}
	
	if ( nWnd >= m_nTotalWnd )
	{
		return nRet ;
	}

	return m_videoWnd[nWnd].ConnectRealPlay(pDev, nChannel, m_checkOsd.GetCheck());
}

bool CClientDemo2Dlg::DealwithAlarm(long lDevcID,  char* pBuf , DWORD dwLen)
{
	SDK_AlarmInfo alarmInfo;
	memcpy ( &alarmInfo, pBuf, dwLen );		
	if ( SDK_EVENT_CODE_NET_ALARM == alarmInfo.iEvent 
		|| SDK_EVENT_CODE_MANUAL_ALARM == alarmInfo.iEvent 
		|| SDK_EVENT_CODE_LOCAL_ALARM == alarmInfo.iEvent )
	{
		AfxMessageBox("1");
		
	}
	else if ( SDK_EVENT_CODE_VIDEO_MOTION == alarmInfo.iEvent ) 
	{
	//	AfxMessageBox("MotionAlarm");
	}
	else if ( SDK_EVENT_CODE_VIDEO_LOSS == alarmInfo.iEvent  )
	{
	//	AfxMessageBox("LossAlarm");
	}
	else if ( SDK_EVENT_CODE_VIDEO_BLIND == alarmInfo.iEvent )
	{
	//	AfxMessageBox("BlindAlarm");
	}
	else if ( SDK_EVENT_CODE_STORAGE_FAILURE == alarmInfo.iEvent )
	{
		AfxMessageBox("StorageFailureAlarm");
	}
	else if ( SDK_EVENT_CODE_LOW_SPACE == alarmInfo.iEvent  )
	{
		AfxMessageBox("LowSpaceAlarm");
	}

	return TRUE;
}

void CClientDemo2Dlg::DrawActivePage(BOOL bActive)
{
	CRect rt;
	m_videoWnd[m_nCurIndex].GetWindowRect(&rt);
	ScreenToClient(&rt);
	
	if ( !bActive )
		InvalidateRect(&rt);		
	else
	{
		CDC *pDC = NULL;	
		pDC=GetDC();
		if (!pDC) 
		{
			return;
		}
		
		pDC->Draw3dRect(&rt, RGB(255,255,0), RGB(255,255,0)); 
		ReleaseDC(pDC);
	}
}

BOOL CClientDemo2Dlg::ExitSDK()
{
	H264_DVR_Cleanup();

	return true;
}

Devc_Map *CClientDemo2Dlg::GetDeviceMap()
{
	return m_devDLg.GetDeviceMap();
}

//message callback function
bool __stdcall MessCallBack(long lLoginID, char *pBuf,
							unsigned long dwBufLen, long dwUser)
{
	return ((CClientDemo2Dlg*)dwUser)->DealwithAlarm(lLoginID,pBuf,dwBufLen);
	
}


void __stdcall DisConnectBackCallFunc(LONG lLoginID, char *pchDVRIP, 
									  LONG nDVRPort, DWORD dwUser)
{
	CClientDemo2Dlg* pThis = (CClientDemo2Dlg*)dwUser;
	if (pThis == NULL)
	{
		ASSERT( FALSE );
		return ;
	}
	pThis->ReConnect(lLoginID, pchDVRIP, nDVRPort);
}


BOOL CClientDemo2Dlg::InitSDK()
{
	//initialize
	BOOL bResult = H264_DVR_Init((fDisConnect)DisConnectBackCallFunc,(DWORD)this);

	//he messages received in SDK from DVR which need to upload， such as alarm information，diary information，may do through callback function
	H264_DVR_SetDVRMessCallBack(MessCallBack,(DWORD)this);
		 
	H264_DVR_SetConnectTime(5000, 3);

	return bResult;
}

void CClientDemo2Dlg::KeyBoardMsg(DWORD dwValue, DWORD dwState)
{
	//get handle of current playing window
	long lPlayHandle = m_videoWnd[m_nCurIndex].GetHandle();
	if ( lPlayHandle <= 0 )
	{
		return;
	}

	SDK_NetKeyBoardData vKeyBoardData;
	vKeyBoardData.iValue = dwValue;
	vKeyBoardData.iState = dwState;
	m_nCurIndex = m_nCurIndex < 0 ? 0 : m_nCurIndex;
	if(!H264_DVR_ClickKey(m_videoWnd[m_nCurIndex].m_lLogin, &vKeyBoardData))
			AfxMessageBox(_CS("AccountMSG.Failed"));
}

void CClientDemo2Dlg::NetAlarmMsg(DWORD dwValue, DWORD dwState)
{
	if (m_devInfo.lLoginID > 0)
	{
		SDK_NetAlarmInfo vAlarmInfo;
		vAlarmInfo.iEvent = 0;
		vAlarmInfo.iState = dwState << dwValue;
		m_nCurIndex = m_nCurIndex < 0 ? 0 : m_nCurIndex;
		if(!H264_DVR_SendNetAlarmMsg(m_devInfo.lLoginID, &vAlarmInfo))
				AfxMessageBox(_CS("AccountMSG.Failed"));
	}
}

void CClientDemo2Dlg::OnBtnCapture() 
{
	// TODO: Add your control notification handler code here
	long lPlayHandle = m_videoWnd[m_nCurIndex].GetHandle();
	if ( lPlayHandle <= 0 )
	{
		return;
	}

	char cFilename[256];

#if 1
	CTime time = CTime::GetCurrentTime();
	sprintf(cFilename, "%s\\Picture\\%4d%02d%02d_%02d%02d%02d.bmp", 
		"c:", 
		time.GetYear(), 
		time.GetMonth(), 
		time.GetDay(), 
		time.GetHour(), 
		time.GetMinute(), 
		time.GetSecond());

	if ( H264_PLAY_CatchPic(m_nCurIndex, cFilename) )
	{
		Sleep(50);
		HINSTANCE hinstance=ShellExecute(m_hWnd, "open" ,cFilename, 
			NULL, NULL, SW_SHOWNORMAL);
	}
	else
	{
		MessageBox(_CS("Desktop.SnapshotFail"));
	}
#else
	CTime time = CTime::GetCurrentTime();
	sprintf(cFilename, "%s\\Picture\\%4d%02d%02d_%02d%02d%02d.jpg", 
		"c:", 
		time.GetYear(), 
		time.GetMonth(), 
		time.GetDay(), 
		time.GetHour(), 
		time.GetMinute(), 
		time.GetSecond());
	if ( H264_DVR_CatchPic(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel, cFilename) )
	{
		Sleep(50);
		HINSTANCE hinstance=ShellExecute(m_hWnd, "open" ,cFilename, 
			NULL, NULL, SW_SHOWNORMAL);
	}
	else
	{
		MessageBox(_CS("Desktop.SnapshotFail"));
	}
#endif
}

void CClientDemo2Dlg::OnBtnDevice() 
{
	// TODO: Add your control notification handler code here
	m_devDLg.ShowWindow(SW_SHOW);
//	m_colorDLg.ShowWindow(SW_HIDE);
//	m_ptzDLg.ShowWindow(SW_HIDE);
//	m_kbDLg.ShowWindow(SW_HIDE);
	m_NetAlarmDLg.ShowWindow(SW_HIDE);
}

void CClientDemo2Dlg::OnBtnNetAlarm() 
{
	// TODO: Add your control notification handler code here
	m_devDLg.ShowWindow(SW_HIDE);
//	m_colorDLg.ShowWindow(SW_HIDE);
//	m_ptzDLg.ShowWindow(SW_HIDE);
//	m_kbDLg.ShowWindow(SW_HIDE);
	m_NetAlarmDLg.m_pDevInfo = &m_devInfo;
	m_NetAlarmDLg.ShowWindow(SW_SHOW);	
}

void CClientDemo2Dlg::OnBtnSave() 
{
	// TODO: Add your control notification handler code here
	m_videoWnd[m_nCurIndex].SaveRecord();
}

void CClientDemo2Dlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	ExitSDK();
}

void CClientDemo2Dlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}

void CClientDemo2Dlg::PtzControl(DWORD dwBtn, bool dwStop)
{
	//get handle of current playing window
	long lPlayHandle = m_videoWnd[m_nCurIndex].GetHandle();
	if ( lPlayHandle <= 0 )
	{
		return;
	}

	// TODO: Add your message handler code here and/or call default
	switch(dwBtn)
	{
	case 0:        //PTZ up
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel, TILT_UP, dwStop))
		{
			AfxMessageBox(_CS("AccountMSG.Failed"));
		}
		break;
	case 1:			//PTZ down
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel,TILT_DOWN,dwStop))
			AfxMessageBox(_CS("AccountMSG.Failed"));
		break;
	case 2:			//PTZ left
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel,PAN_LEFT,dwStop))
			AfxMessageBox(_CS("AccountMSG.Failed"));
		break;
	case 3:			//PTZ right
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel,PAN_RIGHT,dwStop))
			AfxMessageBox(_CS("AccountMSG.Failed"));
		break;
	case 4:         //Zoom In
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel,ZOOM_IN,dwStop))
			AfxMessageBox(_CS("AccountMSG.Failed"));
		break;
	case 5:			//Zoom Out
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel,ZOOM_OUT,dwStop))
			AfxMessageBox(_CS("AccountMSG.Failed"));
		break;
	case 6:			//Focus Near
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel,FOCUS_NEAR,dwStop))
			AfxMessageBox(_CS("AccountMSG.Failed"));
		break;
	case 7:			//Focus Far
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel,FOCUS_FAR,dwStop))
			AfxMessageBox(_CS("AccountMSG.Failed"));
		break;
	case 8:			//Iris Open
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel,IRIS_OPEN,dwStop))
			AfxMessageBox(_CS("AccountMSG.Failed"));
		break;
	case 9:			//Iris Close
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel,IRIS_CLOSE,dwStop))
			AfxMessageBox(_CS("AccountMSG.Failed"));
		break;
	case 10:			//PTZ right top
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel,PAN_RIGTHTOP,dwStop))
			AfxMessageBox(_CS("AccountMSG.Failed"));
		break;
	case 11:			//PTZ left down
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel,PAN_LEFTDOWN,dwStop))
			AfxMessageBox(_CS("AccountMSG.Failed"));
		break;
	case 12:			//PTZ left top
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel,PAN_LEFTTOP,dwStop))
			AfxMessageBox(_CS("AccountMSG.Failed"));
		break;
	case 13:			//PTZ right down
		if(!H264_DVR_PTZControl(m_videoWnd[m_nCurIndex].m_lLogin, 
			m_videoWnd[m_nCurIndex].m_iChannel,PAN_RIGTHDOWN,dwStop))
			AfxMessageBox(_CS("AccountMSG.Failed"));
		break;
	case 14:			//Point Move 
		{

			break;
		}
		
	case 15:			//Point Set 
		{

			break;
		}
	case 16:			//Point Delete
		{

			break;
		}
	case 17:			//Begin Point Loop
		{
			

				break;
		}
	case 18:			//Stop Point Loop
		{
		
			break;
		}
	case 19:			//Show the Cruise Edit Dialog
		{	

			break;
		}

	case 86:			//Add Point To Loop
		{

			break;
		}
	case 87:			//Delete Point From Loop
		{

			break;
		}
	case 88:			//Close Loop
		{

			break;
		}
	default:
		break;
	}
}

void CClientDemo2Dlg::ReConnect(LONG lLoginID, char *pchDVRIP,  LONG nDVRPort)
{
	//clear all play window of this device
	for (int i = 0; i < MAXWNDNUM; i++)
	{
		if (lLoginID == m_videoWnd[i].GetLoginHandle())
		{
			m_videoWnd[i].OnDisconnct();
		}	
	}
	//do reconnect
	m_devDLg.ReConnect(lLoginID, pchDVRIP, nDVRPort);
}

void CClientDemo2Dlg::SetActiveWnd(int nIndex)
{
	if ( -1 != m_nCurIndex )
	{
		DrawActivePage(FALSE);
	}

	m_nCurIndex = nIndex;

	DrawActivePage(TRUE);
	
	SetColor(m_nCurIndex);
}

void CClientDemo2Dlg::SetColor(DWORD nBright, DWORD nContrast, DWORD nSaturation, DWORD nHue)
{
	long lPlayHandle = m_videoWnd[m_nCurIndex].GetHandle();
	if ( lPlayHandle <= 0 )
	{
		return;
	}
	//m_videoWnd[m_nCurIndex].SetColor(nBright, nContrast, nSaturation, nHue);
	SetDevChnColor(nBright, nContrast, nSaturation, nHue);
}


void CClientDemo2Dlg::SetColor( int nIndex )
{
	long nBright = 0;
	long nHue = 0;
	long nSaturation = 0;
	long nContrast = 0;

	long lPlayHandle = m_videoWnd[m_nCurIndex].GetHandle();
	if ( lPlayHandle <= 0 )
	{
		return;
	}
	m_videoWnd[nIndex].GetColor(&nBright, &nContrast, &nSaturation, &nHue);
//	m_colorDLg.SetColor(nBright, nContrast, nSaturation, nHue);
}

BOOL CClientDemo2Dlg::SetDevChnColor(DWORD nBright, DWORD nContrast, DWORD nSaturation, DWORD nHue)
{
	SDK_CONFIG_VIDEOCOLOR videocolor;
	memset(&videocolor,0,sizeof(videocolor));
	
	for ( int i = 0 ;i < 1 ; i ++)
	{
		videocolor.dstVideoColor[i].tsTimeSection.enable = 1;
		videocolor.dstVideoColor[i].tsTimeSection.startHour = 0;
		videocolor.dstVideoColor[i].tsTimeSection.startMinute = 0;
		videocolor.dstVideoColor[i].tsTimeSection.startSecond = 0;
		videocolor.dstVideoColor[i].tsTimeSection.endHour = 24;
		videocolor.dstVideoColor[i].tsTimeSection.endMinute = 0;
		videocolor.dstVideoColor[i].tsTimeSection.endSecond = 0;
		videocolor.dstVideoColor[i].iEnable = 1;
		videocolor.dstVideoColor[i].dstColor.nBrightness = nBright * 100 / 128;
		videocolor.dstVideoColor[i].dstColor.nHue = nHue * 100 / 128;
		videocolor.dstVideoColor[i].dstColor.nSaturation =nSaturation * 100 /128;
		videocolor.dstVideoColor[i].dstColor.nContrast = nContrast * 100 / 128;
		videocolor.dstVideoColor[i].dstColor.mGain = 0;
		videocolor.dstVideoColor[i].dstColor.mWhitebalance = 0;
	}
	
	m_videoWnd[m_nCurIndex].SetDevChnColor(&videocolor);

	return TRUE;
}

void CClientDemo2Dlg::SetDevInfo(DEV_INFO *pDev)
{
	if (pDev)
	{
		memcpy(&m_devInfo, pDev, sizeof(DEV_INFO));
	}
}

void CClientDemo2Dlg::OnSelchangeCombowndnum() 
{
	// TODO: Add your control notification handler code here
}

void CClientDemo2Dlg::OnBtnPlayback() 
{
	// TODO: Add your control notification handler code here
	CLocalpalyback dlg;
	dlg.DoModal();
}

void CClientDemo2Dlg::ArrayWindow1(WORD iNumber)
{
	m_nTotalWnd = iNumber;

	int i = 0;
	CRect Rect;
	GetClientRect(&Rect);
	WORD iWidth,iHeight;

	int nFullWidth=Rect.Width();
	int nFullHeight=Rect.Height();
	iWidth = (int) nFullWidth*0.75515625;
	iHeight = (int)nFullHeight*0.91;
	for (i = 0;i < MAXWNDNUM;i++)
	{
	   m_videoWnd[i].ShowWindow(SW_HIDE);
	}
	
	int nNull = 3;	

	m_videoWnd[iNumber].MoveWindow(3 + 0, 0, iWidth, iHeight);
	m_videoWnd[iNumber].ShowWindow(SW_SHOW);


}


void CClientDemo2Dlg::OnButtonAssure() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_VideoPath == "")
	{
		return ;
	}
	m_videoWnd[m_nCurIndex].URLplay(m_VideoPath);

	
}

void CClientDemo2Dlg::OnButtonStop() 
{
	// TODO: Add your control notification handler code here
	for (int i = 0; i < 4; i++)
	{
		m_videoWnd[i].URLstop();	
	}
	
}
