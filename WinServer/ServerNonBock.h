#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "framework.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <vector>
#include <list>

class ServerNonBlock
{
public:
	ServerNonBlock()
	{
		WSAStartup( MAKEWORD( 2, 2 ), &wsadata );
		s = socket( AF_INET, SOCK_STREAM, 0 );

		addr.sin_family = AF_INET;
		addr.sin_port = htons( 20 );
		addr.sin_addr.S_un.S_addr = inet_addr( "127.0.0.1" );
		bind( s, (LPSOCKADDR)&addr, sizeof( addr ) );
		if ( listen( s, clientMax ) == SOCKET_ERROR )
		{
			exit( 1 );
		}

		size = sizeof( cAddr );

	}
	~ServerNonBlock()
	{
		closesocket( s );
		WSACleanup();
	}

	void InitSocket(HWND hWnd)
	{
		WSAAsyncSelect( s, hWnd, WM_ASYNC, FD_ACCEPT );
	}

	SOCKET AcceptSocket( HWND hWnd )
	{
		SOCKET cs;
		int size;
		size = sizeof( cAddr );
		cs = accept( s, (LPSOCKADDR)&cAddr, &size );
		WSAAsyncSelect( cs, hWnd, WM_ASYNC, FD_READ|FD_CLOSE );

		socketList.push_back( cs );

		return cs;
	}
	void SendMessageToClient( )
	{
		for ( auto& cs : socketList )
		{
			send( cs, (LPSTR)buffer, strlen(buffer)+1, 0 );
		}
	}
	void ReadMessageFromClient(  )
	{
		for ( const auto& cs : socketList )
		{
			int msgLen = recv( cs, buffer, bufferSize, 0 );
			if ( msgLen > 0 )
			{
				buffer[msgLen] = NULL;
#ifdef _UNICODE
				msgLen = MultiByteToWideChar( CP_ACP, 0, buffer, strlen( buffer ), 0, 0 );
				MultiByteToWideChar( CP_ACP, 0, buffer, strlen( buffer ), message, msgLen );
				message[msgLen] = NULL;
#else
				strcpy_s( msg, buffer );

#endif // _UNICODE

				SendMessageToClient();
			}
		}
	}
	void PrintText( HDC hdc )
	{
		TextOut( hdc, 0, 0, message, (int)_tcslen( message ) );
	}
	void CloseClient( SOCKET socket )
	{
		for ( auto it = socketList.begin(); it != socketList.end(); ++it )
		{
			closesocket( cs );
			it = socketList.erase( it );
			break;
		}
	}
	
public:
	static constexpr auto WM_ASYNC = WM_USER + 1;
private:
	static constexpr int clientMax = 5;
	static constexpr int bufferSize = 128;


	WSADATA wsadata;
	SOCKET s;
	SOCKET cs;
	SOCKADDR_IN addr = { 0 };
	SOCKADDR_IN cAddr = { 0 };
	TCHAR message[bufferSize * 2];

	std::list<SOCKET> socketList;

	int size;
	int msgLen;
	char buffer[bufferSize];
};