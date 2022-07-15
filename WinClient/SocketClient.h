#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "framework.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

class SocketClient
{
public:
	SocketClient()
	{
		WSAStartup( MAKEWORD( 2, 2 ), &wsadata );

		s = socket( AF_INET, SOCK_STREAM, 0 );

		addr.sin_family = AF_INET;
		addr.sin_port = htons( 20 );
		addr.sin_addr.S_un.S_addr = inet_addr( "127.0.0.1" );
		if ( connect( s, (LPSOCKADDR)&addr, sizeof( addr ) ) == SOCKET_ERROR )
		{
			MessageBox( NULL, _T( "Connect failed" ), _T( "Error" ), MB_OK );
			exit( 0 );
		}
		else
		{
			MessageBox( NULL, _T( "Connect success" ), _T( "Success" ), MB_OK );
		}
	}
	~SocketClient()
	{
		closesocket( s );
		WSACleanup();
	}

	bool TestClient()
	{
		WSADATA wsadata;
		SOCKET s;
		SOCKADDR_IN addr = { 0 };
		WSAStartup( MAKEWORD( 2, 2 ), &wsadata );

		s = socket( AF_INET, SOCK_STREAM, 0 );
		if ( s == INVALID_SOCKET )
		{
			MessageBox( NULL, _T( "Socket failed" ), _T( "Error" ), MB_OK );
			return false;
		}
		else
		{
			MessageBox( NULL, _T( "Socket success" ), _T( "Success" ), MB_OK );
		}

		addr.sin_family = AF_INET;
		addr.sin_port = htons( 20 );
		addr.sin_addr.S_un.S_addr = inet_addr( "127.0.0.1" );

		if ( connect( s, (LPSOCKADDR)&addr, sizeof( addr ) ) == SOCKET_ERROR )
		{
			MessageBox( NULL, _T( "Connect failed" ), _T( "Error" ), MB_OK );
			return false;
		}
		else
		{
			MessageBox( NULL, _T( "Connect success" ), _T( "Success" ), MB_OK );
		}

		send( s, "Çï·Î ¼­¹ö", 11, 0 );

		closesocket( s );
		WSACleanup();
		return true;
	}

	void SendMsg( const char* str, int size )
	{
		send( s, str, size, 0 );
	}

private:
	static constexpr int bufferSize = 128;

	WSADATA wsadata;
	SOCKET s;
	SOCKADDR_IN addr = { 0 };
	TCHAR message[bufferSize * 2];
	int size;
	int msgLen;
	char buffer[bufferSize];

};