
// UdpChatPrDlg.h: 헤더 파일
//

#pragma once
#define BUF_SIZE 1000

// 메시지, 파일 구조체
typedef struct _DataInfo {
	int x;
	char userName[20];
	char data[1000];

} DataInfo;

// CUdpChatPrDlg 대화 상자
class CUdpChatPrDlg : public CDialogEx
{
// 생성입니다.
public:
	CUdpChatPrDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UDPCHATPR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	SOCKET m_ClientSock, m_ServerSock;
	SOCKADDR_IN m_ServAddr, m_ClntAddr;

public:
	DataInfo dataInfo;

	CEdit m_EditClientIP;
	CEdit m_EditServerIP;
	CEdit m_EditClientPort;
	CEdit m_EditServerPort;
	CEdit m_EditMsg;
	CEdit m_EditServerName;

	CListBox m_ListChat; // IDC_CHAT_LIST
	CListBox m_ListUser; // IDC_USER_LIST(접속자 List)

public:
	afx_msg void ConnectBtn();
	afx_msg void SendBtn();
	afx_msg void DisconnectBtn();
	afx_msg void OnEnChangeMessageEdit();
	afx_msg void CreateHorizontalScroll();

	//static UINT Recv(LPVOID _mothod);

	static UINT TotalRecv(LPVOID _mothod);
	//static UINT TotalRecv2(LPVOID _mothod);
	//static UINT TotalClntRecv(LPVOID _mothod);

	BOOL PreTranslateMessage(MSG * pMsg);
};
