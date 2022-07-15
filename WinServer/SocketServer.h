#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "framework.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

class SocketServer
{
public:
	SocketServer()
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
	~SocketServer()
	{
		closesocket( s );
		WSACleanup();
	}
	bool TestServer()
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
		if(bind(s, (LPSOCKADDR)&addr, sizeof(addr)) )
		{
			MessageBox( NULL, _T( "Binding failed" ), _T( "Error" ), MB_OK );
			return false;
		}
		else
		{
			MessageBox( NULL, _T( "Binding success" ), _T( "Success" ), MB_OK );
		}
		if ( listen( s, clientMax ) == SOCKET_ERROR )
		{
			MessageBox( NULL, _T( "Listen failed" ), _T( "Error" ), MB_OK );
			return false;
		}
		else
		{
			MessageBox( NULL, _T( "Listen success" ), _T( "Success" ), MB_OK );
		}

		TCHAR message[256];

		do
		{
			SOCKADDR_IN c_addr;
			int size = sizeof( c_addr );
			auto cs = accept( s, (LPSOCKADDR)&c_addr, &size );
			
			char buffer[bufferSize];
			int msgLen;
			msgLen = recv( cs, buffer, bufferSize, 0 );
			buffer[msgLen] = NULL;

#ifdef _UNICODE
			TCHAR wBuffer[bufferSize];
			msgLen = MultiByteToWideChar( CP_ACP, 0, buffer, strlen( buffer ), 0, 0 );
			MultiByteToWideChar( CP_ACP, 0, buffer, strlen( buffer ), wBuffer, msgLen );
			wBuffer[msgLen] = NULL;
			_stprintf_s( message, _T( "Client Message : %s, 서버 종료?" ), wBuffer );
#else
			sprintf( mesage, _T( "Client Mesage : %s, 서버종료?" ), buffer );

#endif // _UNICODE


		} while (
			MessageBox( NULL, message, _T( "Server message" ), MB_YESNO ) == IDNO
		);

		closesocket( s );
		WSACleanup();
		return true;
	}
	void DoServer()
	{
		
		do
		{
			cs = accept( s, (LPSOCKADDR)&cAddr, &size );
		} while (cs == INVALID_SOCKET);

		msgLen = recv( cs, buffer, bufferSize, 0 );
		buffer[msgLen] = NULL;

#ifdef _UNICODE
		msgLen = MultiByteToWideChar( CP_ACP, 0, buffer, strlen( buffer ), 0, 0 );
		MultiByteToWideChar( CP_ACP, 0, buffer, strlen( buffer ), message, msgLen );
		message[msgLen] = NULL;
#else
		strcpy_s( msg, buffer );

#endif // _UNICODE

}

	void PrintText(HDC hdc)
	{
		TextOut( hdc, 0, 0, message, (int)_tcslen( message ) );
	}

private:
	static constexpr int clientMax = 5;
	static constexpr int bufferSize = 128;

	WSADATA wsadata;
	SOCKET s;
	SOCKET cs;
	SOCKADDR_IN addr = { 0 };
	SOCKADDR_IN cAddr = { 0 };
	TCHAR message[bufferSize * 2];
	int size;
	int msgLen;
	char buffer[bufferSize];
};