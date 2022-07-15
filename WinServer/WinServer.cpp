#include "framework.h"
#include "WinServer.h"
#include "ServerNonBock.h"
#include <string>
#include <memory>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL    CALLBACK    ServerDialogProc( HWND, UINT, WPARAM, LPARAM );

HWND g_DlgHwnd = NULL;
std::unique_ptr<ServerNonBlock> pServer;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    //return server.Server();

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINSERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINSERVER));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINSERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINSERVER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            if ( !IsWindow( g_DlgHwnd ) )
            {
                g_DlgHwnd = CreateDialog( hInst, MAKEINTRESOURCE( IDD_DIALOG_SERVER ), hWnd, ServerDialogProc );
                
                ShowWindow( g_DlgHwnd, SW_SHOW );
            }
        }
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            //server.PrintText( hdc );
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

BOOL CALLBACK ServerDialogProc( HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
    static HWND hList;

    UNREFERENCED_PARAMETER( lParam );
    switch ( iMsg )
    {
    case WM_INITDIALOG:
        {
            hList = GetDlgItem( hDlg, IDC_LIST_Log );
        }
        return TRUE;

    case ServerNonBlock::WM_ASYNC:
        {
            switch ( lParam )
            {
            case FD_ACCEPT:
                pServer->AcceptSocket(  );
                break;
                // 메시지 읽을 떄
            case FD_READ:
                pServer->ReadMessageFromClient();
                SendMessage( hList, LB_ADDSTRING, 0, (LPARAM)pServer->GetMsg() );
                break;
                // client 끊김
            case FD_CLOSE:
                pServer->CloseClient( wParam );
                break;
            }
        }
        break;

    case WM_COMMAND:
        switch ( LOWORD( wParam ) )
        {
        case IDC_BUTTON_START_SERVER:
            {
                pServer = std::make_unique<ServerNonBlock>(hDlg);
            }
            break;
        case IDC_BUTTON_STOP_SERVER:
            {
                pServer.release();
            }
            break;
        case IDC_BUTTON_SEND:
            {
                wchar_t word[128];
                GetDlgItemText( hDlg, IDC_EDIT_SEND, word, 128 );
                pServer->SendServerMessageToClient( word );
            }
            break;
        case IDCLOSE:
        case IDOK:
        case IDCANCEL:
            {
                DestroyWindow( g_DlgHwnd );
                g_DlgHwnd = NULL;
                return TRUE;
            }
            break;
        }
        break;
    }
    return FALSE;
}