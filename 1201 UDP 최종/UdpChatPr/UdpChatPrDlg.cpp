
// UdpChatPrDlg.cpp: 구현 파일
//

/* WinSock */

#include <WinSock2.h>
#include <thread>
using namespace std;

#include "pch.h"
#include "framework.h"
#include "UdpChatPr.h"
#include "UdpChatPrDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define BUF_SIZE 1024


CUdpChatPrDlg::CUdpChatPrDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UDPCHATPR_DIALOG, pParent)
{

}

void CUdpChatPrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESSAGE_EDIT, m_EditMsg);
	DDX_Control(pDX, IDC_CHAT_LIST, m_ListChat);
	DDX_Control(pDX, IDC_USER_LIST, m_ListUser);
	DDX_Control(pDX, IDC_IP_CLIENT, m_EditClientIP);
	DDX_Control(pDX, IDC_IP_SERVER, m_EditServerIP);
	DDX_Control(pDX, IDC_PORT_CLIENT, m_EditClientPort);
	DDX_Control(pDX, IDC_PORT_SERVER, m_EditServerPort);
	DDX_Control(pDX, IDC_NAME_SERVER, m_EditServerName);
	//DDX_Control(pDX, IDC_NAME_CLIENT, m_EditClientName);
}

BEGIN_MESSAGE_MAP(CUdpChatPrDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, &CUdpChatPrDlg::ConnectBtn)
	ON_BN_CLICKED(IDC_SEND_BUTTON, &CUdpChatPrDlg::SendBtn)
	ON_BN_CLICKED(IDC_DISCONNECT_BUTTON, &CUdpChatPrDlg::DisconnectBtn)
	ON_EN_CHANGE(IDC_MESSAGE_EDIT, &CUdpChatPrDlg::OnEnChangeMessageEdit)

END_MESSAGE_MAP()


// CUdpChatPrDlg 메시지 처리기

BOOL CUdpChatPrDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_EditServerIP.SetWindowTextA("192.168.0.90");
	m_EditClientIP.SetWindowTextA("192.168.0.90");
	m_EditServerPort.SetWindowTextA("50000");
	m_EditClientPort.SetWindowTextA("50001");
	m_EditServerName.SetWindowTextA("키위");

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CUdpChatPrDlg::ConnectBtn() { // 연결 버튼 누르면

	CString sip, sport;
	CString cip, cport;
	CString strMsg, m_name, n_name;

	m_EditServerIP.GetWindowTextA(sip);
	m_EditServerPort.GetWindowTextA(sport);
	m_EditClientIP.GetWindowTextA(cip);
	m_EditClientPort.GetWindowTextA(cport);
	int snum = atoi(sport);
	int cnum = atoi(cport);


	if (!sport.IsEmpty()) {
		WSADATA wsaData;  // winsock data 구조체 사용할 것 
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			AfxMessageBox(_T("WSADATA ERROR!"));
		}

		// Server socket 생성
		m_ServerSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (m_ServerSock == INVALID_SOCKET) {
			AfxMessageBox(_T(" Server SOCKET ERROR!"));
		}

		memset(&m_ServAddr, 0, sizeof(m_ServAddr));

		m_ServAddr.sin_family = AF_INET;
		m_ServAddr.sin_addr.s_addr = inet_addr(sip);
		m_ServAddr.sin_port = htons(snum);

		bind(m_ServerSock, (SOCKADDR*)&m_ServAddr, sizeof(m_ServAddr));

		AfxBeginThread(TotalRecv, this);
		AfxMessageBox(_T(" SOCKET "));

	}


	if (!cport.IsEmpty()) {
		// Client socket 생성
		m_ClientSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (m_ClientSock == INVALID_SOCKET) {
			AfxMessageBox(_T(" Client SOCKET ERROR!"));
		}
		memset(&m_ClntAddr, 0, sizeof(m_ClntAddr));
		m_ClntAddr.sin_family = AF_INET;
		m_ClntAddr.sin_addr.s_addr = inet_addr(cip);
		m_ClntAddr.sin_port = htons(cnum);

		AfxBeginThread(TotalRecv, this);

		m_EditServerName.GetWindowTextA(m_name);
		dataInfo.x = 1;
		if (m_EditServerName > 0) {
			strMsg.Format("[%s]", m_name);
			m_ListUser.InsertString(m_ListUser.GetCount(), strMsg);
		}
		strcpy(dataInfo.userName, strMsg);
		sendto(m_ClientSock, (char*)&dataInfo, sizeof(DataInfo), 0, (SOCKADDR*)&m_ClntAddr, sizeof(m_ClntAddr));
		// client sock 에서 serversock으로 전송
	}

}


void CUdpChatPrDlg::SendBtn()
{
	dataInfo.x = 2;
	CString strMsg, m_name;
	m_EditMsg.GetWindowTextA(strMsg);
	m_EditServerName.GetWindowTextA(m_name);

	strMsg.Format("[%s] : %s", m_name, strMsg);
	strcpy(dataInfo.data, strMsg);

	int retval = sendto(m_ClientSock, (char*)&dataInfo, sizeof(DataInfo), 0, (SOCKADDR*)&m_ClntAddr, sizeof(m_ClntAddr));
	m_ListChat.InsertString(m_ListChat.GetCount(), strMsg);
	m_EditMsg.SetWindowText(_T(""));

}



UINT CUdpChatPrDlg::TotalRecv(LPVOID _mothod) {

	CUdpChatPrDlg* pUdpChatprDlg = (CUdpChatPrDlg*)_mothod;
	char message[BUF_SIZE] = { 0. };
	CString strName;
	CString strMessage = _T("");
	int serverSize = sizeof(pUdpChatprDlg->m_ServAddr);
	int len = 0;

	while (1) {
		len = recvfrom(pUdpChatprDlg->m_ServerSock, (char*)&pUdpChatprDlg->dataInfo, sizeof(DataInfo), 0, (SOCKADDR*)&pUdpChatprDlg->m_ServAddr, &serverSize);

		if (len > 0) {
			switch (pUdpChatprDlg->dataInfo.x)
			{
			case 1:
			{
				strName.Format("%s", pUdpChatprDlg->dataInfo.userName);
				pUdpChatprDlg->m_ListUser.InsertString(pUdpChatprDlg->m_ListUser.GetCount(), strName);
				break;			
			}
			case 2:
			{
				strMessage.Format("%s", pUdpChatprDlg->dataInfo.data);
				pUdpChatprDlg->m_ListChat.InsertString(pUdpChatprDlg->m_ListChat.GetCount(), strMessage);
				break;
			}
			default:
				break;
			}
		}
	}

}
// List Box Scroll bar 생성
void CUdpChatPrDlg::CreateHorizontalScroll()
{
	CString str;
	CSize sz;
	int dx = 0;
	CDC *pDC = m_ListChat.GetDC();

	for (int i = 0; i < m_ListChat.GetCount(); i++)
	{
		m_ListChat.GetText(i, str);
		sz = pDC->GetTextExtent(str);

		if (sz.cx > dx)
			dx = sz.cx;
	}
	m_ListChat.ReleaseDC(pDC);

	if (m_ListChat.GetHorizontalExtent() < dx)
	{
		m_ListChat.SetHorizontalExtent(dx);
		ASSERT(m_ListChat.GetHorizontalExtent() == dx);
	}
}

void CUdpChatPrDlg::DisconnectBtn()
{
	closesocket(m_ClientSock);
	closesocket(m_ServerSock);
	WSACleanup();
	AfxMessageBox(_T("연결 해제!"));
}

void CUdpChatPrDlg::OnEnChangeMessageEdit()
{
	CString strMsg;
	GetDlgItemText(IDC_MESSAGE_EDIT, strMsg);
}

// Enter, ESC Key Error 처리
BOOL CUdpChatPrDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN) { // ENTER키 눌릴 시
			SendBtn();
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE) { // ESC키 눌릴 시
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

//UINT CUdpChatPrDlg::Recv(LPVOID _mothod) // Msg recv
//{
//	CUdpChatPrDlg* pUdpChatprDlg = (CUdpChatPrDlg*)_mothod;
//	char message[BUF_SIZE] = { 0. };
//	int serverSize = sizeof(pUdpChatprDlg->m_ServAddr);
//	int len = 0;
//	while (1)
//	{
//		memset(message, 0, sizeof(message));
//		//len = recvfrom(pUdpChatprDlg->m_ServerSock, message, BUF_SIZE, 0,(SOCKADDR*)&pUdpChatprDlg->m_ServAddr, &serverSize);
//		len = recvfrom(pUdpChatprDlg->m_ServerSock, (char*)&pUdpChatprDlg->dataInfo, sizeof(DataInfo), 0,(SOCKADDR*)&pUdpChatprDlg->m_ServAddr, &serverSize);
//
//		if (len > 0)
//		{
//			CString strMessage = _T("");
//			strMessage.Format("%s", pUdpChatprDlg->dataInfo.data);
//			pUdpChatprDlg->m_ListChat.InsertString(pUdpChatprDlg->m_ListChat.GetCount(), strMessage);
//		}
//	}
//	return 0;
//}