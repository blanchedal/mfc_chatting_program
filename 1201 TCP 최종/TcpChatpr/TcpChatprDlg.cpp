
// TcpChatprDlg.cpp: 구현 파일

/* WinSock */
#include <WinSock2.h>
#include <thread>
using namespace std;

#include "pch.h"
#include "framework.h"
#include "TcpChatpr.h"
#include "TcpChatprDlg.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define BUF_SIZE 30000
#pragma warning(disable:4496)



CTcpChatprDlg::CTcpChatprDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TCPCHATPR_DIALOG, pParent)
{

}

void CTcpChatprDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MESSAGE_EDIT, m_EditMsg);
	DDX_Control(pDX, IDC_CHAT_LIST, m_ListChat);
	DDX_Control(pDX, IDC_USER_LIST, m_ListUser);
	DDX_Control(pDX, IDC_EDIT_IP, m_EditIP);
	DDX_Control(pDX, IDC_EDIT_PORT, m_EditPort);
	DDX_Control(pDX, IDC_EDIT_NAME, m_EditName);
	DDX_Control(pDX, IDC_EDIT_FILE, m_EditFile);
}

BEGIN_MESSAGE_MAP(CTcpChatprDlg, CDialogEx)

	ON_BN_CLICKED(IDC_CONNECT_BUTTON, &CTcpChatprDlg::ConnectBtn)
	ON_BN_CLICKED(IDC_DISCONNECT_BUTTON, &CTcpChatprDlg::DisconnectBtn)
	ON_BN_CLICKED(IDC_SEND_BUTTON, &CTcpChatprDlg::SendBtn)
	ON_EN_CHANGE(IDC_MESSAGE_EDIT, &CTcpChatprDlg::OnEnChangeMessageEdit)
	ON_BN_CLICKED(IDC_BUTTON_FILE, &CTcpChatprDlg::FileOpen)
	ON_BN_CLICKED(IDC_BUTTON_FILE_SEND, &CTcpChatprDlg::FileSendFunc)

END_MESSAGE_MAP()


BOOL CTcpChatprDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_EditIP.SetWindowTextA("192.168.0.90");
	m_EditPort.SetWindowTextA("50000");
	m_EditName.SetWindowTextA("키위새");

	return TRUE;
}

void CTcpChatprDlg::ConnectBtn()
{

	CString m_ip, m_port, m_name;
	CString strTemp, strMsg, strName;
	int clntAdrSize;
	char message[BUF_SIZE];

	m_EditIP.GetWindowTextA(m_ip);
	m_EditPort.GetWindowTextA(m_port);
	int nport = atoi(m_port);

	int mSelect = GetCheckedRadioButton(IDC_RADIO_SERVER, IDC_RADIO_CLIENT);

	if (mSelect == IDC_RADIO_SERVER) {
		dataInfo.x = 1;

		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			AfxMessageBox(_T("WSADATA ERROR!"));
		}

		m_ServerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_ServerSock == INVALID_SOCKET) {
			AfxMessageBox(_T(" Server SOCKET ERROR!"));
		}

		memset(&m_ServAddr, 0, sizeof(m_ServAddr));

		m_ServAddr.sin_family = AF_INET;
		m_ServAddr.sin_addr.s_addr = inet_addr(m_ip);
		m_ServAddr.sin_port = htons(nport);

		bind(m_ServerSock, reinterpret_cast<sockaddr*>(&m_ServAddr), sizeof(m_ServAddr));

		if (listen(m_ServerSock, SOMAXCONN) == SOCKET_ERROR) {
			AfxMessageBox(_T(" Server LISTEN ERROR!"));
		}
		else {
			AfxMessageBox(_T(" Server Listen...."));
		}

		AfxBeginThread(AcceptFunc, this);
		AfxBeginThread(TotalServRecv, this);
	}

	if (mSelect == IDC_RADIO_CLIENT) {

		dataInfo.x = 1;

		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			AfxMessageBox(_T("WSADATA ERROR!"));
		}
		m_EditName.GetWindowTextA(m_name);

		m_ClientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (m_ClientSock == INVALID_SOCKET) {
			AfxMessageBox(_T(" Client SOCKET ERROR!"));
		}

		memset(&m_ClntAddr, 0, sizeof(m_ClntAddr));

		m_ClntAddr.sin_family = AF_INET;
		m_ClntAddr.sin_addr.s_addr = inet_addr(m_ip);
		m_ClntAddr.sin_port = htons(nport);

		if (connect(m_ClientSock, reinterpret_cast<sockaddr*>(&m_ClntAddr), sizeof(m_ClntAddr)) == SOCKET_ERROR) {
			AfxMessageBox(_T(" Client CONNECT ERROR!"));
		}
		else {
			AfxMessageBox(_T(" Client Connected...."));
		}

		m_EditName.GetWindowTextA(m_name);

		if (m_EditName > 0) {
			strTemp.Format("[%s]", m_name);
			m_ListUser.InsertString(m_ListUser.GetCount(), strTemp);
		}
		strcpy_s(dataInfo.userName, strTemp);
		send(m_ClientSock, (const char*)&dataInfo, sizeof(DataInfo), 0);

		AfxBeginThread(TotalClntRecv, this);
	}
}


UINT CTcpChatprDlg::AcceptFunc(LPVOID _mothod) {
	CTcpChatprDlg* pTcpChatprDlg = (CTcpChatprDlg*)_mothod;
	CString strTemp, m_name;

	while (1) {

		pTcpChatprDlg->m_EditName.GetWindowTextA(m_name);
		int clntAdrSize = sizeof(pTcpChatprDlg->m_ClntAddr);
		pTcpChatprDlg->asocket = accept(pTcpChatprDlg->m_ServerSock, (SOCKADDR*)&pTcpChatprDlg->m_ClntAddr, &clntAdrSize);

		// 서버 user list 
		pTcpChatprDlg->m_EditName.GetWindowTextA(m_name);

		if (pTcpChatprDlg->m_EditName > 0) {
			strTemp.Format("[%s]", m_name);
			pTcpChatprDlg->m_ListUser.InsertString(pTcpChatprDlg->m_ListUser.GetCount(), strTemp);
		}
		
		pTcpChatprDlg->dataInfo.x = 1;
		strcpy_s(pTcpChatprDlg->dataInfo.userName, strTemp);
		send(pTcpChatprDlg->asocket, (const char*)&pTcpChatprDlg->dataInfo, sizeof(DataInfo), 0);

		if (accept(pTcpChatprDlg->m_ServerSock, (SOCKADDR*)&pTcpChatprDlg->m_ClntAddr, &clntAdrSize) == INVALID_SOCKET) {
			AfxMessageBox(_T(" Server ACCEPT ERROR!"));
		}
	}
	return 0;
}


void CTcpChatprDlg::SendBtn()
{
	CString m_name;
	char message[BUF_SIZE] = {};
	CString strMsg, strTmp = (_T(""));
	int strLen;
	int servsend = 0;
	int clntsend = 0;

	int mSelect = GetCheckedRadioButton(IDC_RADIO_SERVER, IDC_RADIO_CLIENT);
	m_EditName.GetWindowTextA(m_name);

	if (mSelect == IDC_RADIO_SERVER) {
		dataInfo.x = 2;

		m_EditMsg.GetWindowTextA(strMsg);
		strMsg.Format("[%s] :  %s", m_name, strMsg);
		strcpy_s(dataInfo.data, strMsg);

		servsend = send(asocket, (const char*)&dataInfo, sizeof(DataInfo), 0);

		if (servsend > 0) {
			m_ListChat.InsertString(m_ListChat.GetCount(), (LPCTSTR)dataInfo.data);
			CreateHorizontalScroll();
			m_EditMsg.SetWindowText(_T(""));
		}
	}

	if (mSelect == IDC_RADIO_CLIENT) {
		dataInfo.x = 2;

		m_EditMsg.GetWindowTextA(strMsg);
		strMsg.Format("[%s] : %s", m_name, strMsg);
		strcpy_s(dataInfo.data, strMsg);
		clntsend = send(m_ClientSock, (const char*)&dataInfo, sizeof(DataInfo), 0);

		if (clntsend > 0) {
			m_ListChat.InsertString(m_ListChat.GetCount(), (LPCTSTR)dataInfo.data);
			CreateHorizontalScroll();
			m_EditMsg.SetWindowText(_T(""));
		}
	}
}


UINT CTcpChatprDlg::TotalServRecv(LPVOID _mothod) {
	CTcpChatprDlg* pTcpChatprDlg = (CTcpChatprDlg*)_mothod;
	CString strName, strMsg, strTmp;
	int recvresult;
	int totalnum = 0;

	while (1) {
		recvresult = recv(pTcpChatprDlg->asocket, (char*)&pTcpChatprDlg->dataInfo, sizeof(DataInfo), 0);
		if (recvresult > 0) {
			switch (pTcpChatprDlg->dataInfo.x) {
			case 1:
			{
				strName.Format("%s", pTcpChatprDlg->dataInfo.userName);
				pTcpChatprDlg->m_ListUser.InsertString(pTcpChatprDlg->m_ListUser.GetCount(), strName);
				break;
			}
			case 2: // msg
			{
				strTmp.Format("%s", pTcpChatprDlg->dataInfo.data); // 서버에서 온 메시지 받아서 출력
				pTcpChatprDlg->m_ListChat.InsertString(pTcpChatprDlg->m_ListChat.GetCount(), strTmp);
				break;
			}
			case 3:
			{
				FILE *fp;
				fp = fopen(pTcpChatprDlg->dataInfo.userName, "w+"); // 프로그램 파일 위치에 저장

				fputs(pTcpChatprDlg->dataInfo.data, fp);
				fclose(fp);
				break;
			}

			default:
				AfxMessageBox(_T(" Serv Recv default "));
				break;

			}
		}
	}
}

UINT CTcpChatprDlg::TotalClntRecv(LPVOID _mothod) {
	CTcpChatprDlg* pTcpChatprDlg = (CTcpChatprDlg*)_mothod;
	CString strName, strMsg, strTmp;
	int recvresult;
	int totalnum = 0;

	while (1) {
		recvresult = recv(pTcpChatprDlg->m_ClientSock, (char*)&pTcpChatprDlg->dataInfo, sizeof(DataInfo), 0);

		if (recvresult > 0) {

			switch (pTcpChatprDlg->dataInfo.x) {

			case 1:
			{
				strName.Format("%s", pTcpChatprDlg->dataInfo.userName);
				pTcpChatprDlg->m_ListUser.InsertString(pTcpChatprDlg->m_ListUser.GetCount(), strName);
				break;
			}
			case 2:
			{
				strTmp.Format("%s", pTcpChatprDlg->dataInfo.data); // 서버에서 온 메시지 받아서 출력
				pTcpChatprDlg->m_ListChat.InsertString(pTcpChatprDlg->m_ListChat.GetCount(), strTmp);
				break;
			}
			case 3:
			{
				FILE *fp;
				fp = fopen(pTcpChatprDlg->dataInfo.userName, "w+"); // 프로그램 파일 위치에 저장
				fputs(pTcpChatprDlg->dataInfo.data, fp);
				fclose(fp);
				break;
			}
			default:
				AfxMessageBox(_T(" Clnt Recv default "));
				break;
			}
		}
	}
}


void CTcpChatprDlg::FileOpen()
{
	FILE *fp;
	char fileFilter[] = "All Files(*.*)|*.*|";
	CFileDialog fDlg(TRUE, NULL, NULL, OFN_EXPLORER | OFN_HIDEREADONLY, fileFilter);
	int iReturn = fDlg.DoModal();
	CString m_strFileName;
	char myFile[256];

	if (iReturn == IDOK) {
		DataInfo test;
		test.x = 3;
		m_strFileName = fDlg.GetPathName(); // 파일 이름, 위치 받아오기
		m_strFileName.Replace("\\", "\\\\");
		SetDlgItemText(IDC_EDIT_FILE, m_strFileName);

		CreateHorizontalScroll();
		return;
	}
}


void CTcpChatprDlg::FileSendFunc() { // send button
	char buf[BUF_SIZE] = { 0. };
	int readCnt;
	long filesize;
	int mSelect = GetCheckedRadioButton(IDC_RADIO_SERVER, IDC_RADIO_CLIENT);

	if (mSelect == IDC_RADIO_SERVER) {
		DataInfo test;
		test.x = 3;

		GetDlgItemText(IDC_EDIT_FILE, m_strFileName);

		FILE *fp;
		fp = fopen(m_strFileName, "r+");
		CString FileName = m_strFileName.Right(m_strFileName.GetLength() - m_strFileName.ReverseFind('\\') - 1);
		strcpy(test.userName, FileName);   // 구조체에 파일명 입력

		fseek(fp, 0, SEEK_END); // 끝으로 이동
		filesize = ftell(fp); // size 재고
		fseek(fp, 0, SEEK_SET); // 처음으로 이동

		while (1) {
			memset(buf, 0, sizeof(buf));

			readCnt = fread((void*)&buf, 1, sizeof(buf), fp); // 파일 내용 버퍼에 담기
			if (readCnt < sizeof(buf)) {
				strcpy(test.data, buf);
				send(asocket, (const char*)&test, sizeof(DataInfo), 0);
				break;
			}
			send(asocket, (const char*)&test, sizeof(DataInfo), 0);
		}
		fclose(fp);
		m_EditFile.SetWindowText(_T(""));
		return;
	}

	if (mSelect == IDC_RADIO_CLIENT) {
		DataInfo test;
		test.x = 3;

		GetDlgItemText(IDC_EDIT_FILE, m_strFileName);
		FILE *fp;
		fp = fopen(m_strFileName, "r+");
		CString FileName = m_strFileName.Right(m_strFileName.GetLength() - m_strFileName.ReverseFind('\\') - 1);
		strcpy(test.userName, FileName);

		fseek(fp, 0, SEEK_END);
		filesize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		while (1) {
			readCnt = fread((void*)buf, 1, sizeof(buf), fp); // 파일 내용 버퍼에 담기
			if (readCnt < BUF_SIZE) {
				strcpy(test.data, buf);
				send(m_ClientSock, (const char*)&test, sizeof(DataInfo), 0);
				break;
			}
			send(m_ClientSock, (const char*)&test, sizeof(DataInfo), 0);
		}
		fclose(fp);
		m_EditFile.SetWindowText(_T(""));
		return;
	}

}

void CTcpChatprDlg::DisconnectBtn()   // 소켓 해제
{
	closesocket(m_ClientSock);
	closesocket(m_ServerSock);
	WSACleanup();
	AfxMessageBox(_T("연결 해제!"));
}


// List Box Scroll bar 생성
void CTcpChatprDlg::CreateHorizontalScroll()
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

// Enter, ESC Key Error 처리
BOOL CTcpChatprDlg::PreTranslateMessage(MSG* pMsg)
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

void CTcpChatprDlg::OnEnChangeMessageEdit()
{
	CString strMsg;
	GetDlgItemText(IDC_MESSAGE_EDIT, strMsg);
}