#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#define ROWS 20
#define COLS 20
#define CELL_SIZE 20

int gameMap[ROWS][COLS] = { 0 };
int playerRow = 5;
int playerCol = 5;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void gameOver(HWND hWnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{

    gameMap[playerRow][playerCol] = 1;

    // Initialize walls
    for (int x = 0; x < ROWS; x++)
    {
        for (int y = 0; y < COLS; y++)
        {
            if (x == 0 || y == 0 || x == ROWS - 1 || y == COLS - 1)
            {
                gameMap[x][y] = 2;
            }
        }
    }

    const TCHAR CLASS_NAME[] = _T("GDI_MIN_GAME");

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    int windowWidth = COLS * CELL_SIZE;
    int windowHeight = ROWS * CELL_SIZE;
    RECT rect = { 0, 0, windowWidth, windowHeight };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE);

    HWND hWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        _T("Mini Game"),
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,

        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,

        NULL,
        NULL,
        hInstance,
        NULL);

    if (hWnd == NULL)
    {
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
    {
        int newRow = playerRow;
        int newCol = playerCol;

        switch (wParam)
        {
        case VK_LEFT:
            newCol--;
            break;
        case VK_RIGHT:
            newCol++;
            break;
        case VK_UP:
            newRow--;
            break;
        case VK_DOWN:
            newRow++;
            break;
        default:
            break;
        }

        // Check for collision
        int newPositionValue = gameMap[newRow][newCol];

        if (newPositionValue == 2)
        {
            gameOver(hWnd);
            return 0;
        }
        else
        {
            // Update player position
            gameMap[playerRow][playerCol] = 0;
            playerRow = newRow;
            playerCol = newCol;
            gameMap[playerRow][playerCol] = 1;

            InvalidateRect(hWnd, NULL, FALSE);
        }

        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, COLS * CELL_SIZE, ROWS * CELL_SIZE);
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

        // Fill the background
        RECT rc;
        rc.left = 0;
        rc.top = 0;
        rc.right = COLS * CELL_SIZE;
        rc.bottom = ROWS * CELL_SIZE;
        HBRUSH hBrushBackground = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(hdcMem, &rc, hBrushBackground);
        DeleteObject(hBrushBackground);

        // Draw the game map
        for (int x = 0; x < ROWS; x++)
        {
            for (int y = 0; y < COLS; y++)
            {
                int cellValue = gameMap[x][y];
                RECT cellRect;
                cellRect.left = y * CELL_SIZE;
                cellRect.top = x * CELL_SIZE;
                cellRect.right = cellRect.left + CELL_SIZE;
                cellRect.bottom = cellRect.top + CELL_SIZE;

                if (cellValue == 1)
                {
                    HBRUSH hBrushPlayer = CreateSolidBrush(RGB(0, 0, 255));
                    FillRect(hdcMem, &cellRect, hBrushPlayer);
                    DeleteObject(hBrushPlayer);
                }
                else if (cellValue == 2)
                {
                    HBRUSH hBrushWall = CreateSolidBrush(RGB(0, 0, 0));
                    FillRect(hdcMem, &cellRect, hBrushWall);
                    DeleteObject(hBrushWall);
                }
            }
        }

        BitBlt(hdc, 0, 0, COLS * CELL_SIZE, ROWS * CELL_SIZE, hdcMem, 0, 0, SRCCOPY);

        SelectObject(hdcMem, hbmOld);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);

        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

void gameOver(HWND hWnd)
{
    int message = MessageBoxA(hWnd, "You lost!", "Game Over", MB_OK);
    if (message == IDOK)
    {
        DestroyWindow(hWnd);
    }
}