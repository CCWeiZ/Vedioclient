// DeviceEdit.cpp : implementation file
//

#include "stdafx.h"
#include "ClientDemo2.h"
#include "DeviceEdit.h"
#include "Markup.h"
#include "define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeviceEdit dialog


CDeviceEdit::CDeviceEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDeviceEdit::IDD, pParent)
	,m_nDevNum(0)
	,m_lUpgradeHandle(0)
{
	//{{AFX_DATA_INIT(CDeviceEdit)
	m_strDevName = _T("Test");
	m_nPort = 34567;
	m_strPsw = _T("");
	m_nChannel = "4";
	m_strUserName = _T("admin");
	m_strIP = _T("");
	m_DevSerialID = _T("");
	m_HostPort = 0;
	m_strHostIP = _T("");
	m_bIP = FALSE;
	m_bSerialID = FALSE;
	//}}AFX_DATA_INIT

	m_nType = 0;
	memset(&m_devInfo, 0, sizeof(m_devInfo));
}


void CDeviceEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeviceEdit)
	DDX_Control(pDX, IDC_EDIT_DEVICE_NAME, m_EditDevName);
	DDX_Control(pDX, IDC_EDIT_PORT, m_EditPort);
	DDX_Control(pDX, IDC_EDIT_PSW, m_EditPsw);
	DDX_Control(pDX, IDC_EDIT_TOTALCHANNEL, m_EditTotal);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_EditUserName);
	DDX_Control(pDX, IDC_LIST_Upgrade, m_listDevice);
	DDX_Text(pDX, IDC_EDIT_SVR, m_editIP);
	DDX_Text(pDX, IDC_EDIT_HOSTIP, m_strHostIP);
	DDX_Text(pDX, IDC_EDIT_HOSTPORT, m_HostPort);
	DDX_Text(pDX, IDC_EDIT_DEVICE_NAME, m_strDevName);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUserName);
	DDX_Text(pDX, IDC_EDIT_TOTALCHANNEL, m_nChannel);
	DDX_Text(pDX, IDC_EDIT_SVR, m_strIP);
	DDX_Text(pDX, IDC_EDIT_PSW, m_strPsw);
	DDX_Check(pDX, IDC_CHECK_IP, m_bIP);
	DDX_Check(pDX, IDC_CHECK_SerialID, m_bSerialID);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDIT_SERIALID, m_DevSerialID);
	DDX_Text(pDX, IDC_EDIT_HOSTPORT, m_HostPort);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeviceEdit, CDialog)
	//{{AFX_MSG_MAP(CDeviceEdit)
	ON_WM_DESTROY()
	//ON_BN_DOUBLECLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
ON_BN_CLICKED(IDOK, CDeviceEdit::OnBnClickedOk)
ON_BN_CLICKED(IDCANCEL, CDeviceEdit::OnBnClickedCancel)
ON_BN_CLICKED(IDC_BTN_Search, CDeviceEdit::OnBnClickedBtnSearch)
ON_WM_TIMER()
ON_BN_CLICKED(IDC_BTN_Setting, CDeviceEdit::OnBnClickedBtnSetting)
ON_NOTIFY(NM_DBLCLK, IDC_LIST_Upgrade, CDeviceEdit::OnNMDblclkListUpgrade)
ON_NOTIFY(NM_CLICK, IDC_LIST_Upgrade, CDeviceEdit::OnNMClickListUpgrade)
ON_WM_QUERYDRAGICON()
ON_WM_PAINT()

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeviceEdit message handlers


void CDeviceEdit::DelArpEntry(DWORD dwAddr)
{
	
}

//DEL void CDeviceEdit::OnBTNSearch() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	
//DEL }

void CDeviceEdit::OnBnClickedBtnSearch()
{
	m_nDevNum = 0;
	int count = m_listDevice.GetItemCount(); 
	
	for ( int i = 0; i < count; i ++ )
	{
		DEV_INFO *pData = (DEV_INFO *)m_listDevice.GetItemData(i);
		if ( pData )
		{
			delete pData;
		}
	}
	m_listDevice.DeleteAllItems();
	
	//清除Map
	m_mapDev.clear();
	memset( &m_mapDev[0],0,sizeof(DEV_INFO));
	
	
	GetDlgItem(IDC_BTN_Search)->EnableWindow(FALSE);

//	m_PromptDlg.ShowMsg(true);
	
	DWORD dwThreadID;
	m_hSearch = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)SearchDevice, this , 0, &dwThreadID );
	
	//HCURSOR hCursor;
	//hCursor = LoadCursorFromFile(GET_MODULE_FILE_INFO.strPath + _T("Wait.ani"));//  AfxGetApp()->LoadCursor(IDC_CURSOR1);
	//hCursor=LoadCursorFromFile(_T("res/Wait.ani"));
	//SetCursor(hCursor);
	SetCursor(LoadCursor(NULL, IDC_WAIT));
	CWnd::SetCapture();
	
	//m_PromptDlg.ShowMsg(false);
	
	//	ReleaseCapture();
}
void CDeviceEdit::OnBnClickedCancel()
{
	OnCancel();
}

void CDeviceEdit::OnBnClickedOk()
{
	UpdateData();
	BOOL bIPExists=FALSE;


	if(m_bSerialID)
	{
		int maxDeviceNum = 100;  //最大支持设备数量100
		DDNS_INFO *pDDNSInfo = new DDNS_INFO[maxDeviceNum];
		SearchMode searchmode;
		int nReNum = 0;  //实际获得的设备数量
		
		searchmode.nType = DDNS_SERIAL;
		
		strcpy(searchmode.szSerIP,m_strHostIP.GetBuffer(0));
		searchmode.nSerPort = m_HostPort;
		strcpy(searchmode.szSerialInfo, m_DevSerialID.GetBuffer(0));
		CString strTemp;
		bool bret = H264_DVR_GetDDNSInfo(searchmode, pDDNSInfo, maxDeviceNum, nReNum);
		if ( !bret )
		{
			delete pDDNSInfo;
			pDDNSInfo = NULL;
			return;
		}
		m_strIP=pDDNSInfo[0].IP;
		m_nPort=pDDNSInfo[0].MediaPort;
		UpdateData(FALSE);
	}
	
	//*******************************************************************************************

	
	if ( m_nPort <= 0 )
	{
		MessageBox(_CS("Error.PortInvalid"));
		return;
	}
	if ( m_strDevName == "" )
	{
		MessageBox(_CS("DeviceManage.NameInvalid"));
		return;
	}
	if ( m_strUserName == "" )
	{
		MessageBox(_CS("Error.UserNameInvalid"));
		return;
	}
	CMarkup xml;
	xml.Load(GET_MODULE_FILE_INFO.strPath + "UserInfo.xml");
	while(xml.FindChildElem("ip"))
	{
		xml.IntoElem();
		CString IP="";
		xml.FindChildElem("ip2");
		IP=xml.GetChildData();
		xml.OutOfElem();
		if (IP==m_strIP)
		{
			MessageBox(_CS("Error.IPExits"));
			bIPExists=TRUE;
			OnBnClickedCancel();
			break;
		}
	}

	if(bIPExists==FALSE)
	{
		H264_DVR_DEVICEINFO OutDev;
		int nError = 0;
		//设置尝试连接设备次数和等待时间
		H264_DVR_SetConnectTime(3000, 1);//设置尝试连接1次，等待时间3s
		long lLogin = H264_DVR_Login(m_strIP.GetBuffer(0), m_nPort, m_strUserName.GetBuffer(0), 
			m_strPsw.GetBuffer(0), &OutDev, &nError );
		if ( lLogin <= 0 )
		{
			int nErr = H264_DVR_GetLastError();  
			if(nErr == H264_DVR_PASSWORD_NOT_VALID)
			{
				MessageBox(_CS("Error.PwdErr"));
			}
			else
			{
				MessageBox(_CS("Error.NotFound"));
			}
			return;
		}
		m_devInfo.nTotalChannel = OutDev.byChanNum+OutDev.iDigChannel;
		m_devInfo.lLoginID = lLogin;
		m_devInfo.nPort = m_nPort;
		strcpy(m_devInfo.szDevName, m_strDevName);
		strcpy(m_devInfo.szUserName, m_strUserName);
		strcpy(m_devInfo.szPsw, m_strPsw);
		strcpy(m_devInfo.szIpaddress, m_strIP.GetBuffer(0));

		if(m_bSerialID)
		{
			m_devInfo.bSerialID=m_bSerialID;		
			m_devInfo.nSerPort=m_HostPort;
			strcpy(m_devInfo.szSerIP,m_strHostIP.GetBuffer(0));		
			strcpy(m_devInfo.szSerialInfo, m_DevSerialID.GetBuffer(0));
		}
	

		WriteXML();
		H264_DVR_SetupAlarmChan(lLogin);
	}
    this->OnOK();
}



void CDeviceEdit::OnBnClickedBtnSetting()
{
	int nSelect = m_listDevice.GetSelectionMark();
	if (nSelect != -1)
	{
		DEV_INFO *pData = (DEV_INFO *)m_listDevice.GetItemData(nSelect);
		if ( pData )
		{
//			CIPEdit dlg;
//			memcpy(&dlg.m_NetComm, &pData->NetComm, sizeof(SDK_CONFIG_NET_COMMON));
//			if ( dlg.DoModal() == IDOK )
//			{
//				int nRet = H264_DVR_SetConfigOverNet(E_SDK_CONFIG_SYSNET, -1, (char *)&dlg.m_NetComm, sizeof(dlg.m_NetComm), 5000);
//				if (nRet == H264_DVR_NOERROR || nRet == H264_DVR_SUCCESS)
//				{
//					MessageBox(_CS(_T("Error.SaveSuccess")), _CS(_T("Upgrade.Prompt")));
//				}
//				{
//					MessageBox(_CS(_T("Upgrade.NeedReboot")), _CS(_T("Upgrade.Prompt")));
//				}
//				else
//				{
//					MessageBox(_CS(_T("Error.SetConfigFail")), _CS(_T("Upgrade.Prompt")));
//				}
//			}
		}
	}
}

UINT CDeviceEdit::SearchDevice(LPVOID parm)
{
	CDeviceEdit *pThis = ( CDeviceEdit* ) parm;
	
	int nRetLength = 0;
	bool bRet = H264_DVR_SearchDevice( (char *)&pThis->m_Device, sizeof(pThis->m_Device), &nRetLength, 5000 );
	
	if ( bRet && nRetLength > 0 )
	{
		pThis->m_nDevNum = nRetLength / sizeof(SDK_CONFIG_NET_COMMON_V2);	
	}
	CloseHandle(pThis->m_hSearch);
	pThis->m_hSearch = NULL;
	
	pThis->SetTimer(2, 0, NULL);
	return 0;
}

void CDeviceEdit::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
}

BOOL CDeviceEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	_CWndCS(this);
	
	m_EditUserName.SetLimitText(8);
	m_EditTotal.SetLimitText(2);
	m_EditPsw.SetLimitText(8);
	m_EditPort.SetLimitText(5);
	m_EditDevName.SetLimitText(60);
	
	if ( 1 == m_nType ) //m_nType初始化为零，条件不成立
	{
		m_nPort = m_devInfo.nTotalChannel;
		m_strDevName = m_devInfo.szDevName;
		m_strUserName = m_devInfo.szUserName;
		m_strPsw = m_devInfo.szPsw;
		m_nChannel = m_devInfo.nTotalChannel;
		
		UpdateData(FALSE);
	}

	AfxGetApp()->DoWaitCursor(0);

	//给列表添加列
	m_listDevice.InsertColumn(0, _CS(_T("Upgrade.NO")), LVCFMT_LEFT, 80);
	m_listDevice.InsertColumn(1, _CS(_T("Upgrade.IPAdress")), LVCFMT_LEFT, 200);
	m_listDevice.InsertColumn(2, _CS(_T("Upgrade.Port")), LVCFMT_LEFT, 200);

	m_listDevice.SetExtendedStyle( m_listDevice.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES );

	
	m_strHostIP=_T("10.6.1.35");
	m_HostPort=15000;

		
	m_DevSerialID=_T("1");
	UpdateData(FALSE);
//	m_PromptDlg.Create(IDD_DLG_PROMPT, this);

	m_bIP=TRUE;
	GetDlgItem(IDC_EDIT_HOSTPORT)->EnableWindow(FALSE);//不允许编辑
	GetDlgItem(IDC_EDIT_HOSTIP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SERIALID)->EnableWindow(FALSE);
	UpdateData(FALSE);// 


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeviceEdit::OnNMClickListUpgrade(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nIndex = m_listDevice.GetSelectionMark();
	if ( nIndex >= 0 )
	{	
		DEV_INFO *pData = (DEV_INFO *)m_listDevice.GetItemData( nIndex );
		if ( pData )
		{
			CDeviceEdit dlg;
			//dlg.m_bModify = true;
			memcpy( &dlg.m_devInfo, pData, sizeof(DEV_INFO) );
			m_nPort=pData->nPort;
			CString str_IP;
			str_IP.Format(_T("%d.%d.%d.%d"),pData->NetComm.HostIP.c[0],pData->NetComm.HostIP.c[1],pData->NetComm.HostIP.c[2],pData->NetComm.HostIP.c[3]);
			m_strDevName = str_IP;
			m_strIP=str_IP;
			m_strUserName=pData->szUserName;
			m_strPsw=pData->szPsw;
			UpdateData(FALSE);
		}
	}
	*pResult = 0;
}

void CDeviceEdit::OnPaint()
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
//		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CDeviceEdit::OnQueryDragIcon()
{
	
	return (HCURSOR) m_hIcon;
}

//DEL void CDeviceEdit::OnTimer(UINT nIDEvent) 
//DEL {
//DEL 	// TODO: Add your message handler code here and/or call default
//DEL 	
//DEL 	CDialog::OnTimer(nIDEvent);
//DEL }


void CDeviceEdit::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent==1 )
	{
		KillTimer(1);
		
//		m_PromptDlg.ShowMsg(true);
		
		//HCURSOR hCursor;
		//hCursor = LoadCursorFromFile(GET_MODULE_FILE_INFO.strPath+ _T("Wait.ani"));//  AfxGetApp()->LoadCursor(IDC_CURSOR1);
		//hCursor=LoadCursorFromFile(_T("res/Wait.ani"));
		//SetCursor(hCursor);
		SetCursor(LoadCursor(NULL, IDC_WAIT));
		//CWnd::SetCapture();
	}
	else if ( nIDEvent == 2 )
	{
		GetDlgItem(IDC_BTN_Search)->EnableWindow(TRUE);
		KillTimer(2);
		
	//	m_PromptDlg.ShowMsg(false);
		
		int count = m_listDevice.GetItemCount();
		int i = 0; 
		//check
		for ( i = 0; i < count; i ++ )
		{
			CString strIp;
			CString strIp2;
			int nPort;
			int nPort2;
			strIp.Format(_T("%d.%d.%d.%d"), m_Device[i].HostIP.c[0], m_Device[i].HostIP.c[1],m_Device[i].HostIP.c[2], m_Device[i].HostIP.c[3]);
			nPort=m_Device[i].TCPPort;
			
			for ( int j = i + 1; j < count; j ++ )
			{
				strIp2.Format(_T("%d.%d.%d.%d"), m_Device[j].HostIP.c[0], m_Device[j].HostIP.c[1],
					m_Device[j].HostIP.c[2], m_Device[j].HostIP.c[3]);
				nPort2 = m_Device[j].TCPPort;
				
				if ( strIp2 == strIp && nPort2 == nPort )
				{
					memset( &m_Device[j], 0, sizeof(SDK_CONFIG_NET_COMMON_V2) );
				}
			}
		}
		
		int nIndex = 0;
		for (i = 0 ; i < m_nDevNum; i ++ )
		{
			if ( m_Device[i].TCPPort == 0 || m_Device[i].HostIP.l == 0 )
			{
				continue;
			}
//			CString strTemp(_T(""));
//			strTemp.Format(_T("%d"), count + nIndex + 1);
//			m_listDevice.InsertItem(count + nIndex, strTemp);
//			
//			strTemp.Format(_T("%d.%d.%d.%d"), m_Device[i].HostIP.c[0], m_Device[i].HostIP.c[1],m_Device[i].HostIP.c[2], m_Device[i].HostIP.c[3]);
//			m_listDevice.SetItemText(count + nIndex, 1, strTemp);
//			strTemp.Format(_T("%ld"), m_Device[i].TCPPort );
//			m_listDevice.SetItemText(count + nIndex, 2, strTemp );
			
			//Edit Device Information
			
			DEV_INFO *pData = new DEV_INFO;
			memset( pData, 0, sizeof(DEV_INFO));
			
			pData->nPort = m_Device[i].TCPPort;
			sprintf(pData->szIpaddress, "%d.%d.%d.%d", m_Device[i].HostIP.c[0], m_Device[i].HostIP.c[1],
				m_Device[i].HostIP.c[2], m_Device[i].HostIP.c[3]);
			//sprintf(pData->szUserName, "%s", "admin");
			strcpy(pData->szUserName,"admin");
			//sprintf(pData->szPsw, "%s", "");
			strcpy(pData->szPsw,"");
			memcpy(&pData->NetComm, &m_Device[i], sizeof(SDK_CONFIG_NET_COMMON_V2));
			
			//过滤已经存在的IP和mac相同的设备
			int nFlag = 0;
			if ( strcmp(m_mapDev[0].szIpaddress, "") )
			{
				int j = 0;
				std::map<int, DEV_INFO>::iterator iter = m_mapDev.begin();
				for ( ; iter != m_mapDev.end(); iter ++)
				{
					DEV_INFO pDevInfo;
					memset( &pDevInfo, 0 ,sizeof(DEV_INFO));
					memcpy( &pDevInfo ,&m_mapDev[j],sizeof(DEV_INFO));
					if ( !strcmp( pDevInfo.szIpaddress, pData->szIpaddress) 
						&& !strcmp( pDevInfo.NetComm.sMac , pData->NetComm.sMac))
					{
						nFlag = 1; //存在相同的IP或者Mac
						break;
					}
					j ++;
				}
			}
			
//			pData->nListNum = count + nIndex;
//			
//			m_listDevice.SetItemData( count + nIndex,(long)pData );
//			
//			nIndex ++;
			if ( nFlag )
			{
				delete pData;
				pData = NULL;
				continue;
			}else
			{
				CString strTemp(_T(""));
				strTemp.Format(_T("%d"), count + nIndex + 1);
				m_listDevice.InsertItem(count + nIndex, strTemp);
				
				strTemp.Format(_T("%d.%d.%d.%d"), m_Device[i].HostIP.c[0], m_Device[i].HostIP.c[1],m_Device[i].HostIP.c[2], m_Device[i].HostIP.c[3]);
				m_listDevice.SetItemText(count + nIndex, 1, strTemp);
				strTemp.Format(_T("%ld"), m_Device[i].TCPPort );
				m_listDevice.SetItemText(count + nIndex, 2, strTemp );
				
				pData->nListNum = count + nIndex;
				
				memcpy( &m_mapDev[ nIndex ], pData, sizeof(DEV_INFO));
				
				m_listDevice.SetItemData(count + nIndex,(long)pData );
				
				nIndex ++;
				
			}
		}
		ReleaseCapture();  //释放鼠标
	}
	//close
	else if ( nIDEvent == 3 )
	{
		KillTimer( 3 );
		H264_DVR_CloseUpgradeHandle( m_lUpgradeHandle );
	}
	CDialog::OnTimer(nIDEvent);
}

void CDeviceEdit::OnNMDblclkListUpgrade(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

void CDeviceEdit::OnOk() 
{
	// TODO: Add your control notification handler code here

	CDialog::OnOK();
}

BOOL CDeviceEdit::WriteXML()
{
	CMarkup xml;
	if(xml.Load(GET_MODULE_FILE_INFO.strPath + "UserInfo.xml"))
	{
		xml.FindChildElem("ip");
		if (m_devInfo.szIpaddress!=xml.GetChildData())
		{
			xml.AddChildElem( "ip",m_devInfo.szIpaddress);
			xml.IntoElem();
			xml.AddChildElem("ip2",m_devInfo.szIpaddress);
			xml.AddChildElem("DEVICENAME",m_devInfo.szDevName);
			xml.AddChildElem("nTotalChannel",m_devInfo.nTotalChannel);
			xml.AddChildElem("username",m_devInfo.szUserName);
			xml.AddChildElem("port",m_devInfo.nPort);
			xml.AddChildElem("pwd",m_devInfo.szPsw);
			xml.AddChildElem("byChanNum",m_devInfo.nTotalChannel);
			xml.AddChildElem("lID",m_devInfo.lID);

			xml.AddChildElem("bSerialID",m_devInfo.bSerialID);
			xml.AddChildElem("szSerIP",m_devInfo.szSerIP);
			xml.AddChildElem("nSerPort",m_devInfo.nSerPort);
			xml.AddChildElem("szSerialInfo",m_devInfo.szSerialInfo);
			
		}
		xml.Save(GET_MODULE_FILE_INFO.strPath + "UserInfo.xml");
	}
	else
	{
		xml.AddElem( "INFO" );
		xml.AddChildElem( "ip",m_devInfo.szIpaddress);
		xml.IntoElem();
		xml.AddChildElem("ip2",m_devInfo.szIpaddress);
		xml.AddChildElem("DEVICENAME",m_devInfo.szDevName);
		xml.AddChildElem("nTotalChannel",m_devInfo.nTotalChannel);
		xml.AddChildElem("username",m_devInfo.szUserName);
		xml.AddChildElem("port",m_devInfo.nPort);
		xml.AddChildElem("pwd",m_devInfo.szPsw);
		xml.AddChildElem("byChanNum",m_devInfo.nTotalChannel);
		xml.AddChildElem("lID",m_devInfo.lID);

		xml.AddChildElem("bSerialID",m_devInfo.bSerialID);
		xml.AddChildElem("szSerIP",m_devInfo.szSerIP);
		xml.AddChildElem("nSerPort",m_devInfo.nSerPort);
		xml.AddChildElem("szSerialInfo",m_devInfo.szSerialInfo);
		xml.Save(GET_MODULE_FILE_INFO.strPath + "UserInfo.xml");
	}
	return TRUE;
}
