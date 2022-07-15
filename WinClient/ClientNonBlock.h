#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "framework.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <vector>
#include <list>

class ClientNonBlock
{
public:
	ClientNonBlock(HWND hwnd)
		:
		hWnd(hWnd)
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
		InitSocket( hwnd );
	}
	~ClientNonBlock()
	{
		closesocket( s );
		WSACleanup();
	}

	void InitSocket( HWND hWnd )
	{
		WSAAsyncSelect( s, hWnd, WM_ASYNC, FD_READ );
	}

	bool SendMessageToServer()
	{
		if ( s == INVALID_SOCKET )
		{
			return false;
		}

#ifdef _UNICODE
		msgLen = WideCharToMultiByte( CP_ACP, 0, str, -1, NULL, 0, NULL, NULL );
		WideCharToMultiByte( CP_ACP, 0, str, -1, buffer, msgLen, NULL, NULL );
		message[msgLen] = NULL;
#else
		strcpy_s( buffer, str );
		msgLen = strlen( buffer );
#endif // _UNICODE

		send( s, (LPSTR)buffer, msgLen + 1, 0 );
		msgCount = 0;
		
		return true;
	}

	bool SendMessageToServer(const wchar_t* sendMsg)
	{

		if ( s == INVALID_SOCKET )
		{
			return false;
		}

		msgLen = WideCharToMultiByte( CP_ACP, 0, sendMsg, -1, NULL, 0, NULL, NULL );
		WideCharToMultiByte( CP_ACP, 0, sendMsg, -1, buffer, msgLen, NULL, NULL );

		send( s, (LPSTR)buffer, msgLen + 1, 0 );
		msgCount = 0;

		return true;
	}

	
	void PrintText( HDC hdc )
	{
		TextOut( hdc, 10, 10, str, _tcslen( str ) );
	}

	void PrintServerMessage( HDC hdc )
	{
		if ( _tcscmp( message, _T( "" )) )
		{
			TextOut( hdc, 10, 50, message, _tcslen( message ) );
		}
	}

	void GetServerMessage( )
	{
		msgLen = recv( s, buffer, bufferSize, 0 );
		buffer[msgLen] = NULL;
#ifdef _UNICODE
		msgLen = MultiByteToWideChar( CP_ACP, 0, buffer, strlen( buffer ), 0, 0 );
		MultiByteToWideChar( CP_ACP, 0, buffer, strlen( buffer ), message, msgLen );
		message[msgLen] = NULL;
#else
		strcpy_s( msg, buffer );

#endif // _UNICODE

	}

	void SetStr( WPARAM wParam )
	{
		str[msgCount++] = wParam;
		str[msgCount] = NULL;
	}

	const TCHAR* GetMsg() const
	{
		return message;
	}

public:
	static constexpr int WM_ASYNC = WM_USER + 1;
private:
	static constexpr int clientMax = 5;
	static constexpr int bufferSize = 128;


	WSADATA wsadata;
	SOCKET s;
	SOCKADDR_IN addr = { 0 };
	TCHAR message[bufferSize * 2];
	TCHAR str[bufferSize];
	HWND hWnd;

	int size;
	int msgLen;
	int msgCount = 0;
	char buffer[bufferSize];
};