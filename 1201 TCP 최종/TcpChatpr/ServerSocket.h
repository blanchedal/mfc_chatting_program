#pragma once
class ServerSocket
{
private:
	SOCKET m_ClientSock, m_ServerSock;
	SOCKADDR_IN m_ServAddr, m_ClntAddr;



public:
	CListBox m_ListChat;
	CListBox m_ListUser;

	void ServRecvFunc(CString strMsg, int size);
};

