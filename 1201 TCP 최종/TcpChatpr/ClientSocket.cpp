/* WinSock */
#include <WinSock2.h>
#include <thread>
using namespace std;

#include "pch.h"
#include "ClientSocket.h"
#include "ServerSocket.h"
#include "TcpChatprDlg.h"

void ClientSocket::ClntRecvFunc(CString strMsg, int size) { // server socket ¹ÞÀ½
		//CString strMsg, strTmp;
		ServerSocket* pServerSock = new ServerSocket;
		int recvlen = recv(m_ServerSock, reinterpret_cast<char*>(strMsg.GetBuffer()), strMsg.GetLength() * 2, 0);
		if (recvlen > 0) {
			strMsg.Format("RecvFunc >> %s", strMsg);
			m_ListChat.InsertString(m_ListChat.GetCount(), strMsg);
		}
}