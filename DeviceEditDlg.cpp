// DeviceEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientDemo2.h"
#include "DeviceEditDlg.h"
#include "ClientDemo2Dlg.h"
#include "Markup.h"
#include "DeviceEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeviceEditDlg dialog


CDeviceEditDlg::CDeviceEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeviceEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeviceEditDlg)
	//}}AFX_DATA_INIT
	m_pTalkDecodeBuf = NULL;	//buffer audio data
	m_lTalkHandle = 0;
	memset(&m_talkDevice, 0, sizeof(m_talkDevice));
   
	Devc_Map::iterator p=m_devMap.begin();	
}


void CDeviceEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeviceEditDlg)
	DDX_Control(pDX, IDC_TREE1, m_deviceTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeviceEditDlg, CDialog)
	//{{AFX_MSG_MAP(CDeviceEditDlg)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, OnRclickTree1)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnDblclkTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, OnClickTree1)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_ADD_DEVICE, AddDevice)
	ON_COMMAND(ID_DELETE_DEVICE, DeleteDevice)
	ON_COMMAND(ID_CONNNECT, Connect)
	ON_COMMAND(ID_START_TALK, OnTalk)
	ON_COMMAND(ID_DEVICE_CAPTURE, OnDeviceCapture)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeviceEditDlg message handlers


void CDeviceEditDlg::ReConnect(LONG lLoginID, char *pchDVRIP,  LONG nDVRPort)
{
	//put device into reconnet map
	Devc_Map::iterator p = m_devMap.begin();
	while(p!=m_devMap.end())
	{
		DEV_INFO *pDev = p->second;
		if (pDev->lLoginID == lLoginID)
		{
			H264_DVR_Logout(pDev->lLoginID);
			pDev->lLoginID=-1;
			m_devReconnetMap[pDev->lID] = pDev;
			break;
		}
		p++;
	}

	//start timer
	SetTimer(1, 30000, NULL);
}

void CDeviceEditDlg::Connect()
{
	HTREEITEM hSelectItem = m_deviceTree.GetSelectedItem();
	
	if ( hSelectItem )
	{
		HTREEITEM parentItem = m_deviceTree.GetParentItem(hSelectItem);
		if ( parentItem )
		{
			//if has parent node, is channel node
			DEV_INFO *pDev = (DEV_INFO *)m_deviceTree.GetItemData(parentItem);
			if ( pDev )
			{
				int nChannel = m_deviceTree.GetItemData(hSelectItem);
				
				//connet channel
				CClientDemo2Dlg *pParent = (CClientDemo2Dlg *)GetParent();
				if ( pParent )
				{
					int nRet = pParent->Connect(pDev, nChannel);
					if(nRet<=0)
					{
						return;
					}
				}
			}
			return;
		}
	}
	
	//connect channel selected
	if ( hSelectItem )
	{
		DEV_INFO *pDev = (DEV_INFO *)m_deviceTree.GetItemData(hSelectItem);
		if ( pDev )
		{
			for ( int i = 0; i < pDev->nTotalChannel; i ++ )
			{
				CClientDemo2Dlg *pParent = (CClientDemo2Dlg *)GetParent();
				if ( pParent  )
				{
					int nRet = pParent->Connect(pDev, i, i);
					if ( 0 == nRet )
					{
						//connnect fail, break;
						break;
					}
				}
			}
		}
	}
}

void CDeviceEditDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	Devc_Map::iterator p = m_devReconnetMap.begin();
	while(p!=m_devReconnetMap.end())
	{
		DEV_INFO *pDev = p->second;
		long loginID=DevLogin(pDev);
		if (loginID >0)
		{					
			pDev->lLoginID=loginID;
			m_devReconnetMap.erase(p++);
		}
		else
		{
			p++;
		}
	}
	if (0 == m_devReconnetMap.size())
	{
		KillTimer(1);
	}
	CDialog::OnTimer(nIDEvent);
}


long CDeviceEditDlg::DevLogin(DEV_INFO* pdev)
{


	if(pdev->bSerialID)//���֮ǰ��DDNS��ȡip;�����Ȼ�ȡ��̬ip
	{
		int maxDeviceNum = 100;  //���֧���豸����100
		DDNS_INFO *pDDNSInfo = new DDNS_INFO[maxDeviceNum];
		SearchMode searchmode;
		int nReNum = 0;  //ʵ�ʻ�õ��豸����		
		searchmode.nType = DDNS_SERIAL;		
		strcpy(searchmode.szSerIP,pdev->szSerIP);
		searchmode.nSerPort = pdev->nSerPort ;
		strcpy(searchmode.szSerialInfo, pdev->szSerialInfo);
		CString strTemp;
		bool bret = H264_DVR_GetDDNSInfo(searchmode, pDDNSInfo, maxDeviceNum, nReNum);
		if ( !bret )
		{
			delete pDDNSInfo;
			pDDNSInfo = NULL;
			return FALSE;
		}
		memcpy(pdev->szIpaddress,pDDNSInfo[0].IP,15);
		pdev->nPort=pDDNSInfo[0].MediaPort;
	}

	H264_DVR_DEVICEINFO OutDev;
	int nError = 0;
	//���ó��������豸�����͵ȴ�ʱ��
	H264_DVR_SetConnectTime(3000, 1);//���ó�������1�Σ��ȴ�ʱ��3s
	long lLogin = H264_DVR_Login(pdev->szIpaddress, pdev->nPort, pdev->szUserName, 
		pdev->szPsw, &OutDev, &nError );
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
		return lLogin;
	}
	H264_DVR_SetupAlarmChan(lLogin);
	return lLogin;
}

DEV_INFO CDeviceEditDlg::ReadXML()
{
	CMarkup xml;
	DEV_INFO devInfo;
	
	devInfo.nPort=0;
	devInfo.lLoginID=0;
	devInfo.lID=0;
	
	if(xml.Load(GET_MODULE_FILE_INFO.strPath + "UserInfo.xml"))
	{
		while(xml.FindChildElem("ip"))
		{
			//read the information from XML
			CString strIP="",strUserName="",strPsw="",strDevName="";
			UINT nPort;
			long byChanNum=0,lID=0;

			UINT bSerialID,nSerPort;
			CString szSerIP="",szSerialInfo="";			
			xml.IntoElem();		

			xml.FindChildElem("ip2");
			strIP=xml.GetChildData();
			xml.FindChildElem("DEVICENAME");
			strDevName=xml.GetChildData();
			xml.FindChildElem("username");
			strUserName=xml.GetChildData();
			xml.FindChildElem("port");
			nPort=atoi(xml.GetChildData());
			xml.FindChildElem("pwd");
			strPsw=xml.GetChildData();
			xml.FindChildElem("byChanNum");
			byChanNum=atoi(xml.GetChildData());
			xml.FindChildElem("lID");
			lID=atoi(xml.GetChildData());

			xml.FindChildElem("bSerialID");
			bSerialID=atoi(xml.GetChildData());
			xml.FindChildElem("szSerIP");
			szSerIP=xml.GetChildData();
			xml.FindChildElem("nSerPort");
			nSerPort=atoi(xml.GetChildData());
			xml.FindChildElem("szSerialInfo");
			szSerialInfo=xml.GetChildData();//����ddns��¼
			xml.OutOfElem();
			devInfo.nTotalChannel =byChanNum;
			devInfo.nPort = nPort;

			devInfo.bSerialID=bSerialID;		
			devInfo.nSerPort=nSerPort;
			strcpy(devInfo.szSerIP,szSerIP.GetBuffer(0));		
			strcpy(devInfo.szSerialInfo, szSerialInfo.GetBuffer(0));//����ddns��¼		
			strcpy(devInfo.szDevName, strDevName);
			strcpy(devInfo.szUserName, strUserName);
			strcpy(devInfo.szPsw, strPsw);
			strcpy(devInfo.szIpaddress, strIP);
			DEV_INFO *pDev = new DEV_INFO;
			memset( pDev, 0, sizeof(DEV_INFO) );
			memcpy( pDev, &devInfo, sizeof(DEV_INFO) );
			pDev->lID = (long)pDev;
			m_devMap[pDev->lID] = pDev;
			HTREEITEM hAddItem = m_deviceTree.InsertItem(strDevName);
			m_deviceTree.SetItemData(hAddItem, (DWORD)pDev);
			CString strName("");
			for ( int i = 0; i < byChanNum; i ++)
			{
				strName.Format("CAM %d", i+1);
				HTREEITEM item = m_deviceTree.InsertItem(strName, 0, 0, hAddItem);
				m_deviceTree.SetItemData(item, i);
			}
		}
	}
	return devInfo;
}

BOOL CDeviceEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	ReadXML();
	//return TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeviceEditDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect rect;
	GetClientRect(&rect);
	m_deviceTree.MoveWindow(rect);
	// Do not call CDialog::OnPaint() for painting messages
}

void CDeviceEditDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
		Devc_Map::iterator bIter;
	for ( bIter = m_devMap.begin(); bIter != m_devMap.end(); bIter ++ )
	{
		DEV_INFO *pDev = bIter->second;
		if ( pDev )
		{
			if ( pDev->lLoginID >0)
			{
				H264_DVR_Logout(pDev->lLoginID);
			}
			delete pDev;
			pDev = NULL;
		}
	}
	m_devMap.clear();
	// TODO: Add your message handler code here
	
}

void CDeviceEditDlg::OnRclickTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	CPoint point;
	UINT   pFalg;
	GetCursorPos(&point);
	
	m_deviceTree.ScreenToClient(&point);
	HTREEITEM hitem = m_deviceTree.HitTest(point,&pFalg);
	if (hitem != NULL)
	{
		m_deviceTree.SelectItem(hitem);
	}
	
	HTREEITEM hSelectItem = m_deviceTree.GetSelectedItem();
	if ( hSelectItem )
	{
		HTREEITEM parentItem = m_deviceTree.GetParentItem(hSelectItem);
		if ( parentItem )
		{
			//if has parent node, is channel node
			CMenu Menu;
			Menu.CreatePopupMenu();
			
			Menu.AppendMenu(MF_STRING , ID_CONNNECT, _CS("Desktop.ConnVideo"));
			Menu.AppendMenu(MF_STRING , ID_ADD_DEVICE, _CS("DeviceManage.DevAdd"));
			
			CPoint pt;
			::GetCursorPos(&pt);
			Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
			
			*pResult = 0;
			
			return;
		}
		//device node
		else
		{
			DEV_INFO *pDev = (DEV_INFO *)m_deviceTree.GetItemData(hSelectItem);
			if (pDev->lLoginID<=0)
			{
				CMenu Menu;
				Menu.CreatePopupMenu();
				Menu.AppendMenu(MF_STRING , ID_START_TALK, _CS("Desktop.StartTalk"));
				Menu.AppendMenu(MF_STRING , ID_ADD_DEVICE, _CS("DeviceManage.DevAdd"));
				Menu.AppendMenu(MF_STRING , ID_DELETE_DEVICE, _CS("DeviceManage.DevDel"));

				CPoint pt;
				::GetCursorPos(&pt);
				Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
			}
			
			if ( pDev && pDev->lLoginID > 0 )
			{
				CMenu Menu;
				Menu.CreatePopupMenu();
				
				if ( strcmp(m_talkDevice.szDevName, pDev->szDevName) )
				{
					Menu.AppendMenu(MF_STRING , ID_START_TALK, _CS("Desktop.StartTalk"));
					Menu.AppendMenu(MF_STRING , ID_ADD_DEVICE, _CS("DeviceManage.DevAdd"));
					Menu.AppendMenu(MF_STRING , ID_DELETE_DEVICE, _CS("DeviceManage.DevDel"));
				}
				else
				{
					Menu.AppendMenu(MF_STRING , ID_START_TALK, _CS("Desktop.StopTalk"));
					Menu.AppendMenu(MF_STRING , ID_ADD_DEVICE, _CS("DeviceManage.DevAdd"));
					Menu.AppendMenu(MF_STRING , ID_DELETE_DEVICE, _CS("DeviceManage.DevDel"));
				}
				Menu.AppendMenu(MF_STRING , ID_DEVICE_CAPTURE, _CS("�豸ץͼ"));

				CPoint pt;
				::GetCursorPos(&pt);
				Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);	
				
				return;
			}
		}
	}
	else
	{
		CMenu Menu;
		Menu.CreatePopupMenu();
		Menu.AppendMenu(MF_STRING , ID_ADD_DEVICE, _CS("DeviceManage.DevAdd"));
		CPoint pt;
		::GetCursorPos(&pt);
		Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
	}
}

void CDeviceEditDlg::OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
//	MessageBox("OnDblclkTree1");
	HTREEITEM hSelectItem = m_deviceTree.GetSelectedItem();
	if(hSelectItem)
	{
		HTREEITEM parentItem = m_deviceTree.GetParentItem(hSelectItem);
		if ( parentItem )
		{
			//channel node
			Connect();
			*pResult = 0;
			return;
		}
		//channel node
		else
		{	//login
			DEV_INFO* pDev= (DEV_INFO *)m_deviceTree.GetItemData(hSelectItem);
			if(pDev->lLoginID<=0)
			{				
				long loginID=DevLogin(pDev);
				if (loginID >0)
				{					
					pDev->lLoginID=loginID;
					pDev->lID = (long)pDev;
					m_devMap[pDev->lID] = pDev;
					CClientDemo2Dlg *pParent = (CClientDemo2Dlg *)GetParent();
					if ( pParent)
					{
						pParent->SetDevInfo(pDev);
					}
				}
				else 
				{
                    *pResult=true;
					return;
				}
			}
			//logout
			else
			{
				DEV_INFO* pDev = (DEV_INFO *)m_deviceTree.GetItemData(hSelectItem);
				H264_DVR_Logout(pDev->lLoginID);
				pDev->lLoginID=-1;
				pDev->lID = (long)pDev;
				m_devMap[pDev->lID] = pDev;
			}
		}
	}
	
	*pResult = 0;
}

BOOL CDeviceEditDlg::InputTalkData(BYTE *pBuf, DWORD nBufLen)
{
	return H264_PLAY_InputData( 499, pBuf, nBufLen );
}

void CDeviceEditDlg::OnClickTree1(NMHDR* pNMHDR, LRESULT* pResult) //������ 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSelectItem = m_deviceTree.GetSelectedItem();
	if(hSelectItem)
	{
		DEV_INFO *pDev = NULL;
		HTREEITEM parentItem = m_deviceTree.GetParentItem(hSelectItem);
		if ( parentItem ) //channel node
		{
			//if has parent node, is channel node
			pDev = (DEV_INFO *)m_deviceTree.GetItemData(parentItem);
		}
		else //dev node
		{
			pDev = (DEV_INFO *)m_deviceTree.GetItemData(hSelectItem);
		}
		
		if ( pDev )
		{
			CClientDemo2Dlg *pParent = (CClientDemo2Dlg *)GetParent();
			if ( pParent  )
			{
				pParent->SetDevInfo(pDev);
			}
		}
	}
//	MessageBox("OnClickTree1");
	*pResult = 0;
}

#define MAX_CAPTURE_BUF		100*1024
char g_capturebuf[MAX_CAPTURE_BUF];
void CDeviceEditDlg::OnDeviceCapture()
{
	HTREEITEM hSelectItem = m_deviceTree.GetSelectedItem();
	
	if ( hSelectItem )
	{
		HTREEITEM parentItem = m_deviceTree.GetParentItem(hSelectItem);
		if ( parentItem )
		{
			return;
		}
		else
		{
			DEV_INFO *pDev = (DEV_INFO *)m_deviceTree.GetItemData(hSelectItem);
			if ( pDev && pDev->lLoginID > 0 )
			{
				int nRetLen;
				bool bRet = H264_DVR_CatchPicInBuffer(pDev->lLoginID, 0, g_capturebuf, MAX_CAPTURE_BUF, &nRetLen);
				if ( bRet )
				{
					CString str("c:\\test.jpg");

					CFile file;
					file.Open(str, CFile::modeCreate | CFile::modeReadWrite);

					file.Write(g_capturebuf, nRetLen);
					file.Close();

					CString msg;
					msg.Format("ץͼ�ɹ�  %s", str);
					MessageBox(msg);
				}
				else
				{
					MessageBox("ץͼʧ��");
				}
			}
		}
	}
}

void CDeviceEditDlg::OnTalk()
{
	HTREEITEM hSelectItem = m_deviceTree.GetSelectedItem();
	
	if ( hSelectItem )
	{
		HTREEITEM parentItem = m_deviceTree.GetParentItem(hSelectItem);
		if ( parentItem )
		{
			return;
		}
		else
		{
			DEV_INFO *pDev = (DEV_INFO *)m_deviceTree.GetItemData(hSelectItem);
			if ( pDev && pDev->lLoginID > 0 )
			{
				if ( strcmp(m_talkDevice.szDevName, pDev->szDevName) )
				{
					// if talk already started, stop it first.
					if ( m_talkDevice.lLoginID > 0 )
						StopTalk(&m_talkDevice);
					
					StartTalk(pDev);
					
					memcpy( &m_talkDevice, pDev, sizeof(m_talkDevice));
				}
				else
				{
					StopTalk(pDev);
					
					memset( &m_talkDevice, 0 ,sizeof(m_talkDevice));
				}	
			}
		}
	}
}

void CDeviceEditDlg::DeleteDevice()
{
	HTREEITEM hItem = m_deviceTree.GetSelectedItem();
	
	if ( hItem == NULL )
	{
		return;
	}
	
	DEV_INFO *pDev = (DEV_INFO *)m_deviceTree.GetItemData(hItem);
	if ( pDev )
	{
		if ( pDev->lLoginID > 0 )
		{
			H264_DVR_Logout(pDev->lLoginID);
		}
		
		Devc_Map::iterator bIter = m_devMap.find( pDev->lID );
		if ( bIter != m_devMap.end() )
		{
			m_devMap.erase(bIter);
		}
		m_deviceTree.DeleteItem(hItem);
		CMarkup xml;
		if (xml.Load(GET_MODULE_FILE_INFO.strPath + "UserInfo.xml"))
		{
			while(xml.FindChildElem("ip"))
			{
				xml.IntoElem();
				xml.FindChildElem("ip2");
				if(xml.GetChildData()==pDev->szIpaddress)
				{
					xml.RemoveElem();
					xml.Save(GET_MODULE_FILE_INFO.strPath + "UserInfo.xml");
				}
				xml.OutOfElem();
			}
			
		}
		delete pDev;
		pDev = NULL;
	}	
}

void CDeviceEditDlg::AddDevice()
{
	CDeviceEdit dlg;
	if ( IDOK == dlg.DoModal() )
	{
		DEV_INFO *pDev = new DEV_INFO;
		memset( pDev, 0, sizeof(DEV_INFO) );
		memcpy( pDev, &dlg.m_devInfo, sizeof(DEV_INFO) );
		pDev->lID = (long)pDev;
		m_devMap[pDev->lID] = pDev;
		pDev->nTotalChannel = 4;
		if(pDev->lLoginID>0)
		{
			//add to device tree
			HTREEITEM hAddItem = m_deviceTree.InsertItem(pDev->szDevName);
			m_deviceTree.SetItemData(hAddItem, (DWORD)pDev);
			
			CString strName("");
			for ( int i = 0; i < pDev->nTotalChannel; i ++ )
			{
				strName.Format("CAM %d", i + 1);
				HTREEITEM item = m_deviceTree.InsertItem( strName, 0, 0, hAddItem );
				m_deviceTree.SetItemData(item, i);
			}
			m_deviceTree.SelectItem(hAddItem);
			m_deviceTree.Expand(hAddItem, TVE_EXPAND);
		}
		
	}
}

BOOL CDeviceEditDlg::SendTalkDate(LPBYTE pDataBuffer,DWORD dwDataLength)
{
	if ( NULL == m_pTalkDecodeBuf )
	{
		m_pTalkDecodeBuf = new BYTE[4096];
	}
	memset(m_pTalkDecodeBuf, 0, 4096);
	
	int iCbLen = 0;
	
	//encode talk data
//	if (g711a_Encode( (char*)pDataBuffer , (char*)m_pTalkDecodeBuf+8 , dwDataLength, &iCbLen) != 1)
//	{
//		ASSERT(FALSE);
		return FALSE;
//	}
	
	m_pTalkDecodeBuf[0]=0x00;
	m_pTalkDecodeBuf[1]=0x00;
	m_pTalkDecodeBuf[2]=0x01;
	m_pTalkDecodeBuf[3]=0xFA;
	
	m_pTalkDecodeBuf[4]=0x0E; //G711A
	m_pTalkDecodeBuf[5]=0x02;
	m_pTalkDecodeBuf[6]=BYTE(iCbLen&0xff);
	m_pTalkDecodeBuf[7]=BYTE(iCbLen>>8);	
	
	iCbLen += 8;
	return H264_DVR_VoiceComSendData( m_lTalkHandle, (char*)m_pTalkDecodeBuf, iCbLen );
}

BOOL CDeviceEditDlg::StartTalk(DEV_INFO *pDevice)
{
	if ( !pDevice )
	{
		return false;
	}
	
	// open audio capture
	int nLen = 640;
	DWORD dwSampleRate = 8000;
	DWORD nAudioBit = 16;
	
//	BOOL bRet = H264_PLAY_StartAudioCapture(AudioDataCallBack, nAudioBit, dwSampleRate, nLen, (long)this);
//	if ( !bRet )            
//	{
//		MessageBox(_CS("Error.OpenAudioFail"));
//		return false;
//	}
	
	//open audio talk decode channel
	//audio talk port must be 499
//	bRet = StartTalkPlay(499);
//	if ( !bRet )
//	{
//		MessageBox(_CS("Error.OpenAudioDecodeFail"));
//		return false;
//	}
	
	//request audio talk

	
	return true;
}

BOOL CDeviceEditDlg::StopTalk(DEV_INFO *pDevice)
{
	if ( !pDevice )
	{
		return false;
	}
	//stop audio data capture
	H264_PLAY_StopAudioCapture();
	//close decode channel
	StopTalkPlay(499);	
	
	H264_DVR_StopVoiceCom(m_lTalkHandle);	
	m_lTalkHandle = 0;
	
	return true;
}

BOOL CDeviceEditDlg::StopTalkPlay(int nPort)
{
	BOOL bPlayOk = FALSE;
	bPlayOk = H264_PLAY_CloseStream(nPort);
	bPlayOk &= H264_PLAY_StopSoundShare(nPort);
	bPlayOk &= H264_PLAY_Stop(nPort);
	
	return bPlayOk;
}

