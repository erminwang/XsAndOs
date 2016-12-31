// XsAndOs.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "XsAndOs.h"
#include <windowsx.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    LoadStringW(hInstance, IDC_XSANDOS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_XSANDOS));

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
	HDC hdc;
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_XSANDOS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	//wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 255, 255));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_XSANDOS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//Global
const int CELL_SIZE = 150;
HBRUSH hbr1, hbr2;
int playerTurn = 1;
int gameBoard[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int winner = 0;
int wins[3];
//
//my function  make a structure for the rectangle
BOOL GetGameBoardRect(HWND hwnd, RECT * pRect)
{
	RECT rc;
	if (GetClientRect(hwnd, &rc))     //retrieve the coordinates of a window's client area
	{
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		pRect->left = (width - CELL_SIZE * 3) / 2;
		pRect->top = (height - CELL_SIZE * 3) / 2;
		pRect->right = pRect->left + CELL_SIZE * 3;
		pRect->bottom = pRect->top + CELL_SIZE * 3;
		return TRUE;

	}

	SetRectEmpty(pRect);
	return FALSE;
}

//my function draw line between two points
void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

//my function
/*return -1 if click outside, 0~8 if click inside the rectangle*/
int GetCellNumberFromPoint(HWND hwnd, int x, int y)
{
	POINT pt = { x, y };
	RECT rc;

	//pt.x = x;
	//pt.y = y;
	if (GetGameBoardRect(hwnd, &rc))
	{
		if (PtInRect(&rc, pt))   //user clicks inside game board
		{
			//Normalize (0 to 3*CELL_SIZE)
			x = pt.x - rc.left;
			y = pt.y - rc.top;
			int column = x / CELL_SIZE;
			int row = y / CELL_SIZE;
			//convert to index 0~8
			return column + row * 3;
		}
	}
	return -1;   //if click outside the game board or on failure
}
//my function get dimension from index
BOOL GetCellRect(HWND hwnd, int index, RECT * pRect)
{
	RECT rcBoard;
	SetRectEmpty(pRect);
	if (index < 0 || index > 8)
		return FALSE;
	if (GetGameBoardRect(hwnd, &rcBoard))
	{
		//get x,y coordinates
		int y = index / 3; //Row no.
		int x = index % 3; //Column no.
		pRect->left = rcBoard.left + x * CELL_SIZE + 1;
		pRect->top = rcBoard.top + y * CELL_SIZE + 1;
		pRect->right = pRect->left + CELL_SIZE - 1;
		pRect->bottom = pRect->top + CELL_SIZE - 1;
		return TRUE;
	}
	return FALSE;
}
/*my function for determining who is the winner
0 no winner
1 player wins
2 player wins
3 draw

  0 1 2
  3 4 5
  6 7 8

*/
int GetWinner(int wins[3])
{
	int cells[] = { 0,1,2, 3,4,5, 6,7,8, 0,3,6, 1,4,7, 2,5,8 ,0,4,8, 2,4,6 };
	
	//check for winner
	for (int i = 0; i < ARRAYSIZE(cells); i += 3)
	{
		if (0!=gameBoard[cells[i]] && gameBoard[cells[i]] == gameBoard[cells[i + 1]] && gameBoard[cells[i]] == gameBoard[cells[i + 2]])
		{
			//We get a winner
			wins[0] = cells[i];
			wins[1] = cells[i + 1];
			wins[2] = cells[i + 2];
			return gameBoard[cells[i]];
		}
	}
	//see if we have any cells left
	for (int i = 0; i < ARRAYSIZE(gameBoard); ++i)
	{
		if (0 == gameBoard[i])
			return 0; //the game continues to play
	}
	return 3; //a draw

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_FILE_NEWGAME:
				{
					int ret = MessageBox(hWnd, L"Are you sure you want to start a new game?", L"New Game", MB_YESNO | MB_ICONQUESTION);
					if (IDYES == ret)
					{
						//reset and start a new game
						playerTurn = 1;
						winner = 0;
						ZeroMemory(gameBoard, sizeof(gameBoard));
						//Force a paint message
						InvalidateRect(hWnd, NULL, TRUE); //Post WM_PAINT to our windowProc. It gets queued in our msg queue
						UpdateWindow(hWnd); //forces immediate handling of WM_PAINT
					}
				}
				break;
            case IDM_ABOUT:
                //DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				MessageBox(NULL, L"Ermin Wang's Game\nVisit one-blog.herokuapp.com to see posts", L"About Xs and Os", MB_OK);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_LBUTTONDOWN:  //get information about which cell the user clicks
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			//Only handle clicks if it is a player turn (i.e. 1 or 2)
			if (0 == playerTurn)
				break;
			int index = GetCellNumberFromPoint(hWnd, xPos, yPos);

			HDC hdc = GetDC(hWnd);
			if (NULL != hdc)
			{
				//WCHAR temp[100];            //text GetCellNumberFromPoint function
				//wsprintf(temp, L"Index = %d", index);
				//TextOut(hdc, xPos, yPos, temp, lstrlen(temp));

				//Get cell dimension by index
				if (index != -1)
				{
					RECT rcCell;
					if ((0 == gameBoard[index]) && GetCellRect(hWnd, index, &rcCell))
					{
						gameBoard[index] = playerTurn;
						FillRect(hdc, &rcCell, (playerTurn==2) ? (HBRUSH)CreateSolidBrush(RGB( 255, 0, 255)): (HBRUSH)CreateSolidBrush(RGB(255, 255, 0)));
						//check for winner
						winner = GetWinner(wins);
						if (winner == 1 || winner == 2)
						{
							//we have winner
							MessageBox(hWnd, (winner == 1) ? L"Player 1 wins" : L"Player 2 wins", L"You win", MB_OK | MB_ICONINFORMATION);
							playerTurn = 0;
						}
						else if (3 == winner)
						{
							//It's a draw
							MessageBox(hWnd, L"No one wins", L"It's a draw", MB_OK | MB_ICONEXCLAMATION);
							playerTurn = 0;
						}
						else if (0 == winner)
						{
							playerTurn = (playerTurn == 1) ? 2 : 1;
						}
						
						
					}

					
				}
				ReleaseDC(hWnd, hdc);
			}
			
		}
		break;
	case WM_GETMINMAXINFO:
		{
			MINMAXINFO * pMinMax = (MINMAXINFO*)lParam;
			pMinMax->ptMinTrackSize.x = CELL_SIZE * 5;
			pMinMax->ptMinTrackSize.y = CELL_SIZE * 5;
			pMinMax->ptMaxTrackSize.x = CELL_SIZE * 5;
			pMinMax->ptMaxTrackSize.y = CELL_SIZE * 5;
		}
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
			RECT rc;
			if (GetGameBoardRect(hWnd, &rc))
			{
				//FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
				//Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
				FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

				for (int i = 0; i < 4; ++i)
				{
					//draw vertical lines
					DrawLine(hdc, rc.left + CELL_SIZE * i, rc.top, rc.left + CELL_SIZE * i, rc.bottom);
					//draw horizontal lines
					DrawLine(hdc, rc.left, rc.top + CELL_SIZE * i, rc.right, rc.top + CELL_SIZE * i);
				}
				//Draw the occupied cells if window resized
				RECT rcCell;
				for (int i = 0; i < ARRAYSIZE(gameBoard); ++i)
				{
					if ((0!=gameBoard[i]) && GetCellRect(hWnd, i, &rcCell))
					{
						FillRect(hdc, &rcCell, (gameBoard[i] == 2) ? (HBRUSH)CreateSolidBrush(RGB(255, 0, 255)) : (HBRUSH)CreateSolidBrush(RGB(255, 255, 0)));
					}
				}
			}
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
