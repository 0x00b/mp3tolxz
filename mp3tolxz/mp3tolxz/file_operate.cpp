// file_operate.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "file_operate.h"

#pragma comment(lib,"AdvAPI32.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"Shell32.lib")
#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//���ݶ���
#define XOR_BIT_LEN		512
#define XOR_BIT			0x18
#define SUFIX_LEN		4
#define LXZ_SUFIX		".lxz"
#define MP3_SUFIX		".MP3"
#define MP3_SUFIX_		".mp3"

typedef bool(*pVisitFile)(const char* lpPath);

//��������
bool TraverFolder(pVisitFile visit, const char* lpPath);
bool VisitFile(const char* lpPath);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �ڴ˷��ô��롣

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FILE_OPERATE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FILE_OPERATE));

    MSG msg;

    // ����Ϣѭ��: 
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



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FILE_OPERATE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FILE_OPERATE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   int width = GetSystemMetrics(SM_CXSCREEN);
   int height = GetSystemMetrics(SM_CYSCREEN);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   (width - 600)/2, (height-400)/2,600, 400, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_SELECTDIR:
			case ID_BACKFILE:
				int ret;
				char szPath[MAX_PATH];
				BROWSEINFO bi;
				LPITEMIDLIST pIDL;
				bi.hwndOwner = hWnd;
				bi.pidlRoot = NULL;
				bi.pszDisplayName = szPath;
				bi.lpszTitle = "ѡ���ļ���";
				bi.ulFlags = BIF_RETURNONLYFSDIRS;
				bi.lpfn = NULL;
				bi.lParam = 0;
				bi.iImage = 0;
				pIDL = SHBrowseForFolder(&bi);
				if (SHGetPathFromIDList(pIDL, szPath))
				{
					if (wmId == ID_SELECTDIR)
					{
						ret = MessageBox(hWnd, "ȷ�����ѡ���ļ����е������ļ�ô��", "ȷ��", MB_OKCANCEL);
					}
					else
					{
						ret = MessageBox(hWnd, "ȷ����ԭѡ���ļ����е������ļ�ô��", "ȷ��", MB_OKCANCEL);
					}
					if (ret == IDOK)
					{
						if (TraverFolder(VisitFile, szPath))
						{
							MessageBox(hWnd, "�����ɹ�", "ȷ��", 0);
						}
					}
				}
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
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
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

// �����ڡ������Ϣ�������
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

bool TraverFolder(pVisitFile visit, const char* lpPath)
{
	char szFile[MAX_PATH] = { 0 };
	char szFind[MAX_PATH];
	bool bret = true;
	WIN32_FIND_DATA FindFileData;
	strcpy(szFind, lpPath);

	strcat(szFind, "\\*.*");
	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		return true;
	}
	while (TRUE)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				lstrcpy(szFile, lpPath);
				lstrcat(szFile, "\\");
				lstrcat(szFile, FindFileData.cFileName);
				(!TraverFolder(visit, szFile)) && (bret && (bret = false));
			}
		}
		else
		{
			lstrcpy(szFile, lpPath);
			lstrcat(szFile, "\\");
			lstrcat(szFile, FindFileData.cFileName);
			(!visit(szFile)) && (bret && (bret = false));
		}

		if (!FindNextFile(hFind, &FindFileData))
		{
			break;
		}
	}
	FindClose(hFind);
	return bret;
}

bool VisitFile(const char* lpPath)
{
	if (NULL == lpPath)
	{
		return false;
	}
	FILE* pFile = NULL;
	char buf[XOR_BIT_LEN + 1];
	int readLen = 0;
	int pathLen = lstrlen(lpPath);
	char tmpPath[MAX_PATH];
	bool flag = false;
	if (0 == lstrcmp(lpPath + (pathLen - SUFIX_LEN), MP3_SUFIX)
		|| 0 == lstrcmp(lpPath + (pathLen - SUFIX_LEN), MP3_SUFIX)
		|| 0 == lstrcmp(lpPath + (pathLen - SUFIX_LEN), LXZ_SUFIX))
	{
		strcpy(tmpPath, lpPath);
		if (0 == lstrcmp(lpPath + (pathLen - SUFIX_LEN), LXZ_SUFIX))
		{
			strcpy(tmpPath + strlen(tmpPath) - SUFIX_LEN, MP3_SUFIX);
		}
		else
		{
			strcpy(tmpPath + strlen(tmpPath) - SUFIX_LEN, LXZ_SUFIX);
		}
		pFile = fopen(lpPath, "rb+");
		if (NULL != pFile)
		{
			if (1 == (readLen = fread(buf, XOR_BIT_LEN, 1, pFile)))
			{
				buf[XOR_BIT_LEN] = 0;
				for (int i = 0; i < XOR_BIT_LEN; i++)
				{
					buf[i] ^= XOR_BIT;
				}
				fseek(pFile, 0, 0);
				if (1 != fwrite(buf, XOR_BIT_LEN, 1, pFile))
				{
					char err[1024];
					sprintf(err, "file[%s] write content failed!", lpPath);
					MessageBox(NULL, err, "error", 0);
					return false;
				}
				else
				{
					flag = true;
				}
			}
			else
			{
				char err[1024];
				sprintf(err, "file[%s] read content failed!", lpPath);
				MessageBox(NULL, err, "error", 0);
				return false;
			}
			fclose(pFile);
			if (flag && 0 != rename(lpPath, tmpPath))
			{
				char err[1024];
				sprintf(err, "change file name [%s] to [%s] failed!", lpPath, tmpPath);
				MessageBox(NULL, err, "error", 0);
				return false;
			}
		}
		else
		{
			char err[1024];
			sprintf(err, "open file [%s] failed!", lpPath, tmpPath);
			MessageBox(NULL, err, "error", 0);
			return false;
		}
	}
	return true;
}