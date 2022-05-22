
// TcpChatprDlg.h: 헤더 파일
//

#pragma once
#define BUF_SIZE 30000

// 메시지, 파일 구조체
typedef struct _DataInfo {
	int x;
	char userName[20];
	char data[30000];

} DataInfo;

// CTcpChatprDlg 대화 상자
class CTcpChatprDlg : public CDialogEx
{
// 생성입니다.
public:
	CTcpChatprDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TCPCHATPR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	SOCKET m_ClientSock, m_ServerSock;
	SOCKADDR_IN m_ServAddr, m_ClntAddr;

public:
	SOCKET asocket;
	DataInfo dataInfo;

	CListBox m_ListChat;
	CListBox m_ListUser;

	CEdit m_EditMsg;
	CEdit m_EditIP;
	CEdit m_EditPort;
	CEdit m_EditName;
	CEdit m_EditFile;

	CWinThread* RecvMsg = NULL;
	CWinThread* servRecvMsg = NULL;

	afx_msg void ConnectBtn();
	afx_msg void SendBtn();
	afx_msg void CreateHorizontalScroll();
	afx_msg void OnEnChangeMessageEdit();
	afx_msg void DisconnectBtn();
	afx_msg void FileOpen();
	afx_msg void FileSendFunc();

	static UINT AcceptFunc(LPVOID _mothod);
	static UINT TotalServRecv(LPVOID _mothod);
	static UINT TotalClntRecv(LPVOID _mothod);

	BOOL PreTranslateMessage(MSG * pMsg);
	
};
