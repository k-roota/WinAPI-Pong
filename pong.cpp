// pong.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "pong.h"

#define MAX_LOADSTRING 100

#define WIDTH 500
#define HEIGHT 350
#define TRANSPARENCY 20
#define DIA 16
#define BALL_POS_X 50
#define BALL_POS_Y 50
#define PADDLE_WIDTH 15
#define PADDLE_HEIGHT 60
#define TIME 10 //50
#define START_DIRECTION { 1, 1 }
#define MAX_FILENAME 260
#define POINT_TOP 15
#define POINT_DIGITS 4
#define BALL_SIZING 2
#define TRACE_TIME 2*TIME*DISTANCE/5
#define LEVEL_TIME 1000
#define START_LEVEL 1
#define MAX_WCHAR 256


const double GAP = DIA*(1 / 2 - 1 / (2 * sqrt(2)));
const int POINT_HEIGHT = 80;
const int POINT_WIDTH = WIDTH / 3;
const int TRACE_COUNT = (DIA - 1) / BALL_SIZING;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szBallClass[MAX_LOADSTRING];
WCHAR szBallTraceClass[MAX_LOADSTRING];
WCHAR szPaddleClass[MAX_LOADSTRING];

POINT direction;
HWND hPaddle;
int winCount = 0;
int looseCount = 0;
HWND hBall;
HBITMAP hBitmap = NULL;
HDC hMemDC = NULL;
HBITMAP hBmp = NULL;
HBITMAP hOldBmp = NULL;
COLORREF bkColor = RGB(64, 0, 64);
bool isBitmap = false;
bool isStretched = false;
RECT looseRc;
RECT winRc;
HWND hTrace[TRACE_COUNT];
int lastTrace = 0;
int DISTANCE = START_LEVEL;
bool isPause = true;
bool isLoose = false;
int personalBest = 0;
int gameTime = 0;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK BallProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK PaddleProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK BallTraceProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
ATOM BallRegisterClass(HINSTANCE hInstance);
ATOM PaddleRegisterClass(HINSTANCE hInstance);
ATOM BallTraceRegisterClass(HINSTANCE hInstance);
RECT CollisionDetection(HWND hParent, RECT ballRc, RECT paddleRc);
RECT PosOnParent(HWND hWnd, HWND hParent);
void SetPaddlePos(HWND hWnd, LPARAM lParam);
void NewGame(HWND hWnd, HWND hBall);
void SelectItem(HWND hWnd, UINT uID);
void SetBitmaps(HWND hWnd);
void UpdateBitmap(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB2, szWindowClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_BALL, szBallClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_BALL_TRACE, szBallTraceClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PADDLE, szPaddleClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
	BallRegisterClass(hInstance);
	PaddleRegisterClass(hInstance);
	BallTraceRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2));

    MSG msg;

    // Main message loop:
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(CreateSolidBrush(bkColor));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

ATOM BallRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = BallProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(255, 0, 0)));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szBallClass;
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}

ATOM BallTraceRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = BallTraceProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(255, 0, 0)));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szBallTraceClass;
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}

ATOM PaddleRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = PaddleProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH)(GetSysColorBrush(COLOR_ACTIVECAPTION));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szPaddleClass;
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   RECT rc;
   HRESULT result = SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & (~WS_THICKFRAME) & (~WS_MAXIMIZEBOX) | WS_CLIPCHILDREN,
      (rc.left+rc.right+1-WIDTH)/2, (rc.top+rc.bottom+1-HEIGHT)/2, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

   for (int i = 0; i < TRACE_COUNT; i++)
	   hTrace[i] = NULL;

   if (!hWnd)
   {
      return FALSE;
   }

   SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
   SetLayeredWindowAttributes(hWnd, 0, (255 * (100-TRANSPARENCY)) / 100, LWA_ALPHA);

   hBall = CreateWindow(szBallClass, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
	   BALL_POS_X, BALL_POS_Y, DIA, DIA, hWnd, nullptr, hInst, nullptr);
   HRGN region = CreateEllipticRgn(0, 0, DIA, DIA);
   SetWindowRgn(hBall, region, true);

   GetClientRect(hWnd, &rc);

   hPaddle = CreateWindow(szPaddleClass, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
	   (rc.right-PADDLE_WIDTH), (rc.bottom - PADDLE_HEIGHT) / 2, PADDLE_WIDTH, PADDLE_HEIGHT, hWnd, nullptr, hInst, nullptr);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		looseRc.top = winRc.top = POINT_TOP;
		looseRc.bottom = winRc.bottom = POINT_TOP + POINT_HEIGHT;
		looseRc.left = rc.right / 4 - POINT_WIDTH / 2;
		looseRc.right = rc.right / 4 + POINT_WIDTH / 2;
		winRc.left = rc.right * 1 / 4 - POINT_WIDTH / 2;
		winRc.right = rc.right * 3 / 4 + POINT_WIDTH / 2;
		SetTimer(hWnd, IDT_TIMER3, LEVEL_TIME, NULL);
		HDC hdc = GetDC(hWnd);
		hMemDC = CreateCompatibleDC(hdc);
		ReleaseDC(hWnd, hdc);
		SetBitmaps(hWnd);
		UpdateBitmap(hWnd);
		break;
	}
	case WM_TIMER:
		if (!isPause)
		{
			gameTime += LEVEL_TIME;
			DISTANCE = 3 * pow(gameTime / LEVEL_TIME, 1/3.0) + 1;
		}
		break;
	case WM_MOUSEMOVE:
	{
		SetPaddlePos(hWnd, lParam);
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_FILE_NEWGAME:
				NewGame(hWnd, hBall);
				break;
			case ID_BACKGROUND_COLOR:
			{
				CHOOSECOLOR cc;
				static COLORREF arcCustClr[16];
				ZeroMemory(&cc, sizeof(cc));
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = hWnd;
				cc.lpCustColors = arcCustClr;
				cc.Flags = CC_FULLOPEN | CC_RGBINIT;
				if (ChooseColor(&cc))
				{
					isBitmap = false;
					bkColor = cc.rgbResult;
					UpdateBitmap(hWnd);
					HMENU menu = GetMenu(hWnd);
					EnableMenuItem(menu, ID_BACKGROUND_TILE, MF_GRAYED);
					EnableMenuItem(menu, ID_BACKGROUND_STRECH, MF_GRAYED);
				}
			}
			break;
			case ID_BACKGROUND_BITMAP:
			{
				OPENFILENAME ofn;
				WCHAR szFile[MAX_FILENAME];
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = L"Bitmap (*.bmp)\0*.bmp\0";
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				if (GetOpenFileName(&ofn))
				{
					HMENU menu = GetMenu(hWnd);
					EnableMenuItem(menu, ID_BACKGROUND_TILE, MF_ENABLED);
					EnableMenuItem(menu, ID_BACKGROUND_STRECH, MF_ENABLED);
					if(hBitmap != NULL)
						DeleteObject(hBitmap);
					hBitmap = (HBITMAP)LoadImage(NULL, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
					isBitmap = true;
					SelectItem(hWnd, ID_BACKGROUND_TILE);
				}
			}
			break;
			case ID_BACKGROUND_TILE:
			case ID_BACKGROUND_STRECH:
			{
				SelectItem(hWnd, wmId);
			}
			break;
			case ID_HELP_MANUAL:
				MessageBox(hWnd, L"Don't let the ball to go behind the paddle. Press F2 to start the game.", L"Manual", MB_OK);
				break;
			case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_ERASEBKGND:
		return 1;
	case WM_DISPLAYCHANGE:
		SetBitmaps(hWnd);
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
			RECT rc;
			GetClientRect(hWnd, &rc);
			BitBlt(hdc, 0, 0, rc.right, rc.bottom, hMemDC, 0, 0, SRCCOPY);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		if (hOldBmp != NULL) {
			SelectObject(hMemDC, hOldBmp);
		}
		if (hMemDC != NULL) {
			DeleteDC(hMemDC);
		}
		if (hBmp != NULL) {
			DeleteObject(hBmp);
		}
		if (hBitmap != NULL)
			DeleteObject(hBitmap);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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

LRESULT CALLBACK BallProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
		{
			SetTimer(hWnd, IDT_TIMER1, TIME, NULL);
			SetTimer(hWnd, IDT_TIMER2, TRACE_TIME, NULL);
			direction = START_DIRECTION;
		}
		break;
		case WM_TIMER:
		{
			HWND hParent = GetParent(hWnd);
			RECT ballRc = PosOnParent(hWnd, hParent);
			RECT paddleRc = PosOnParent(hPaddle, hParent);
			switch (wParam)
			{
			case IDT_TIMER1:
				if (!isPause)
				{
					ballRc.left += DISTANCE * direction.x;
					ballRc.right += DISTANCE * direction.x;
					ballRc.top += DISTANCE * direction.y;
					ballRc.bottom += DISTANCE * direction.y;
					ballRc = CollisionDetection(hParent, ballRc, paddleRc);
					if (!isLoose)
					{
						MoveWindow(hWnd, ballRc.left, ballRc.top, DIA, DIA, true);
					}
					isLoose = false;
				}
				break;
			case IDT_TIMER2:
				for (int i = 0; i < TRACE_COUNT; i++)
				{
					if (hTrace[i] != NULL)
					{
						RECT rc;
						GetWindowRgnBox(hTrace[i], &rc);
						if (rc.right - rc.left > BALL_SIZING)
						{
							HRGN hNewRegion = CreateEllipticRgn(rc.left + BALL_SIZING / 2, rc.top + BALL_SIZING / 2, rc.right + 1 - BALL_SIZING / 2, rc.bottom + 1 - BALL_SIZING / 2);
							SetWindowRgn(hTrace[i], hNewRegion, true);
						}
						else
						{
							DestroyWindow(hTrace[i]);
							hTrace[i] = NULL;
						}
					}
				}
				if (!isPause)
				{
					lastTrace = (lastTrace + 1) % TRACE_COUNT;
					hTrace[lastTrace] = CreateWindow(szBallTraceClass, nullptr, WS_CHILD | WS_CLIPSIBLINGS,
						ballRc.left + BALL_SIZING / 2, ballRc.top + BALL_SIZING / 2, DIA - BALL_SIZING, DIA - BALL_SIZING, hParent, nullptr, hInst, nullptr);
					HRGN region = CreateEllipticRgn(BALL_SIZING / 2, BALL_SIZING / 2, DIA - BALL_SIZING / 2, DIA - BALL_SIZING / 2);
					SetWindowRgn(hTrace[lastTrace], region, true);
					ShowWindow(hTrace[lastTrace], SW_SHOW);
				}
			}
			
		}
		break;
		case WM_NCHITTEST:
			return HTTRANSPARENT;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK BallTraceProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NCHITTEST:
		return HTTRANSPARENT;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

RECT PosOnParent(HWND hWnd, HWND hParent)
{
	RECT rc;
	GetWindowRect(hWnd, &rc);
	POINT pos = { rc.left, rc.top };
	POINT corner = { rc.right, rc.bottom };
	ScreenToClient(hParent, &pos);
	ScreenToClient(hParent, &corner);
	rc.left = pos.x;
	rc.top = pos.y;
	rc.right = corner.x;
	rc.bottom = corner.y;
	return rc;
}

RECT CollisionDetection(HWND hParent, RECT ballRc, RECT paddleRc)
{
	bool isPaddle = false;
	RECT parentRc;
	GetClientRect(hParent, &parentRc);
	if (ballRc.right > paddleRc.left && direction.x == 1)
	{
		if ((direction.y == 1 && ballRc.bottom - paddleRc.top - GAP > ballRc.right - paddleRc.left && ballRc.top - paddleRc.bottom + GAP < ballRc.right - paddleRc.left)
			|| (direction.y == -1 && paddleRc.bottom - ballRc.top - GAP > ballRc.right - paddleRc.left && paddleRc.top - ballRc.bottom + GAP < ballRc.right - paddleRc.left))
		{
			parentRc.right = paddleRc.left;
			isPaddle = true;
		}
	}

	if (ballRc.bottom > parentRc.bottom)
	{
		ballRc.bottom = 2 * parentRc.bottom - ballRc.bottom;
		ballRc.top = ballRc.bottom - DIA;
		direction.y = -1;
	}
	else if (ballRc.top < parentRc.top)
	{
		ballRc.top = 2 * parentRc.top - ballRc.top;
		ballRc.bottom = ballRc.top + DIA;
		direction.y = 1;
	}
	if (ballRc.right > parentRc.right)
	{
		ballRc.right = 2 * parentRc.right - ballRc.right;
		ballRc.left = ballRc.right - DIA;
		direction.x = -1;
		if (isPaddle)
		{
			winCount++;
			PlaySound(MAKEINTRESOURCE(IDR_WAVE2), NULL, SND_ASYNC | SND_RESOURCE);
		}
		else
		{
			looseCount++;
			isPause = true;
			isLoose = true;
			ShowWindow(hBall, SW_HIDE);
			PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_ASYNC | SND_RESOURCE);
			if (winCount > personalBest)
			{
				personalBest = winCount;
			}
			wchar_t text[MAX_WCHAR];
			swprintf_s(text, MAX_WCHAR, L"You've lost. Your best score : %d", personalBest);
			MessageBox(hParent, text, L"Important message", MB_OK);
		}
		UpdateBitmap(hParent);
	}
	if (ballRc.left < parentRc.left)
	{
		ballRc.left = 2 * parentRc.left - ballRc.left;
		ballRc.right = ballRc.left + DIA;
		direction.x = 1;
	}
	return ballRc;
}

void SetPaddlePos(HWND hWnd, LPARAM lParam)
{
	int yPos = GET_Y_LPARAM(lParam);
	RECT paddleRc;
	GetWindowRect(hPaddle, &paddleRc);
	POINT point = { paddleRc.left, paddleRc.top };
	ScreenToClient(hWnd, &point);
	point.y = yPos - PADDLE_HEIGHT / 2;
	RECT rc;
	GetClientRect(hWnd, &rc);
	if (point.y < rc.top)
		point.y = rc.top;
	else if (point.y + PADDLE_HEIGHT > rc.bottom)
		point.y = rc.bottom - PADDLE_HEIGHT;
	MoveWindow(hPaddle, point.x, point.y, PADDLE_WIDTH, PADDLE_HEIGHT, true);
}

LRESULT CALLBACK PaddleProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NCHITTEST:
		return HTTRANSPARENT;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void NewGame(HWND hWnd, HWND hBall)
{
	winCount = 0;
	looseCount = 0;
	direction = START_DIRECTION;
	MoveWindow(hBall, BALL_POS_X, BALL_POS_Y, DIA, DIA, true);
	for (int i = 0; i < TRACE_COUNT; i++)
	{
		if (hTrace[i] != NULL)
		{
			DestroyWindow(hTrace[i]);
			hTrace[i] = NULL;
		}
	}
	DISTANCE = START_LEVEL;
	gameTime = 0;
	isPause = false;
	ShowWindow(hBall, SW_SHOW);
	UpdateBitmap(hWnd);
}

void SelectItem(HWND hWnd, UINT uID)
{
	HMENU hMenu = GetMenu(hWnd);
	UINT secondID;
	if (uID == ID_BACKGROUND_TILE)
	{
		secondID = ID_BACKGROUND_STRECH;
		isStretched = false;
	}
	else
	{
		secondID = ID_BACKGROUND_TILE;
		isStretched = true;
	}
	CheckMenuItem(hMenu, secondID, MF_UNCHECKED);
	CheckMenuItem(hMenu, uID, MF_CHECKED);
	UpdateBitmap(hWnd);
}

void SetBitmaps(HWND hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	HDC hdc = GetDC(hWnd);
	if (hOldBmp != NULL) {
		SelectObject(hMemDC, hOldBmp);
	}
	if (hBmp != NULL) {
		DeleteObject(hBmp);
	}
	hBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
	hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmp);
	ReleaseDC(hWnd, hdc);
}

void UpdateBitmap(HWND hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	if (isBitmap)
	{
		if (isStretched)
		{
			HDC hdc = GetDC(hWnd);
			HDC hTmpDC = CreateCompatibleDC(hdc);
			HBITMAP hOldTmpBmp = (HBITMAP)SelectObject(hTmpDC, hBitmap);

			BITMAP bmpInfo;
			GetObject(hBitmap, sizeof(BITMAP), &bmpInfo);
			SetStretchBltMode(hMemDC, HALFTONE);
			StretchBlt(hMemDC, 0, 0, rc.right, rc.bottom, hTmpDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY);

			SelectObject(hTmpDC, hOldTmpBmp);
			DeleteDC(hTmpDC);
			ReleaseDC(hWnd, hdc);
		}
		else
		{
			HBRUSH newBrush = CreatePatternBrush(hBitmap);
			FillRect(hMemDC, &rc, newBrush);
			DeleteBrush(newBrush);
		}
	}
	else
	{
		HBRUSH newBrush = CreateSolidBrush(bkColor);
		FillRect(hMemDC, &rc, newBrush);
		DeleteBrush(newBrush);
	}
	
	SetBkMode(hMemDC, TRANSPARENT);
	HFONT hFont = CreateFont(POINT_HEIGHT, 0, 0, 0, 0, false, FALSE, 0, EASTEUROPE_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Verdana");
	HFONT hOldFont = (HFONT)SelectObject(hMemDC, hFont);
	wchar_t wWinCount[POINT_DIGITS+1];
	_itow_s(winCount, wWinCount, 10);
	SetTextColor(hMemDC, bkColor ^ 0x00FFFFFF);
	DrawText(hMemDC, wWinCount, -1, &winRc, DT_CENTER);

	SelectObject(hMemDC, hOldFont);
	DeleteFont(hFont);

	InvalidateRect(hWnd, NULL, true);
}
