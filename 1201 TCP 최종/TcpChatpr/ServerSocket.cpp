/* WinSock */
#include <WinSock2.h>
#include <thread>
using namespace std;

#include "pch.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "TcpChatprDlg.h"

void ServerSocket::ServRecvFunc(CString strMsg, int size) { // clientsocket ¹ÞÀ½
	//CString strMsg, strTmp;
	//ServerSocket* pServerSock = new ServerSocket;
	CTcpChatprDlg* pTcp = new CTcpChatprDlg;
	ClientSocket* pClientSock = new ClientSocket;
	int recvlen = recv(m_ClientSock, reinterpret_cast<char*>(strMsg.GetBuffer()), strMsg.GetLength() * 2, 0);
	if (recvlen > 0) {
		strMsg.Format("RecvFunc >> %s", strMsg);
		m_ListChat.InsertString(m_ListChat.GetCount(), strMsg);
	}
}