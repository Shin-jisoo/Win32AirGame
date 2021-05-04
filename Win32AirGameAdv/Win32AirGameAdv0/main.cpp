#include <windows.h>
#include <stdio.h>
#include "resource.h"


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE hInst;
HBITMAP hBmpAir, hBmpBack, hBmpBullet, hBmpEnemy, hBmpExplosion;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	int nShowCmd)
{
	HWND hWnd;
	MSG mSg;
	hInst = hInstance;
	char szTitle[] = "Air Plane Game";
	char szClass[] = "Class";

	WNDCLASSEX WndEx;

	WndEx.cbSize = sizeof(WndEx);
	WndEx.style = NULL;
	WndEx.lpfnWndProc = WndProc;
	WndEx.cbClsExtra = 0;
	WndEx.cbWndExtra = 0;
	WndEx.hInstance = hInstance;
	WndEx.hIcon = LoadIcon(NULL, "IDI_ICON");
	WndEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndEx.lpszMenuName = "IDR_MENU";
	WndEx.lpszClassName = szClass;
	WndEx.hIconSm = LoadIcon(hInstance, "");

	RegisterClassEx(&WndEx);
	hWnd = CreateWindowEx(NULL, szClass, szTitle,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		320 * 2, 240 * 2, NULL, NULL, hInstance, NULL);
	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);

	hBmpAir = LoadBitmap(hInstance, "IDB_AIR");
	hBmpBack = LoadBitmap(hInstance, "IDB_BACK");
	hBmpBullet = LoadBitmap(hInstance, "IDB_BULLET");
	hBmpEnemy = LoadBitmap(hInstance, "IDB_ENEMY");
	hBmpExplosion = LoadBitmap(hInstance, "IDB_EXPLOSION");
	InvalidateRect(hWnd, NULL, FALSE);
	while (TRUE)
	{
		if (PeekMessage(&mSg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&mSg, NULL, 0, 0))
				break;
			TranslateMessage(&mSg);
			DispatchMessage(&mSg);
		}
	}
	return mSg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC, hMemDC;
	PAINTSTRUCT pS;
	static int nX = 30, nY = 100, nBX[10], nBY[10];
	static int nXb = 0;
	static int nX1e = 400, nY1e = 80;   //bullet, enemy
	static int nX2e = 400, nY2e = 180;  //bullet2, enemy2
	static bool bullet[10] = { 0,0,0,0,0,0,0,0,0,0 };  //총알 배열 변수
	static bool hit[10] = { 0,0,0,0,0,0,0,0,0,0 }; 
	//static int nBw[10] = { 1,1,1,1,1,1,1,1,1,1 };
	static int gamestart = 0,  max = 9, nb = 0;
	static bool enemy1 = false;
	static bool enemy2 = false;
	static int max_no_of_bullet = 9, bn = 0;
	static int nn = 0, nn2 = 0; //nn2=single multi변수 ,nn2= stop,start변수 
	static int ne = 1; //enemy 1, 2 menu
	int i;
	static int no_of_fire = 0, no_of_hit = 0; //횟수
	char szText[100];
	static int nMx, nMy;

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		hDC = GetDC(hWnd);
		nMx = LOWORD(lParam);
		nMy = HIWORD(lParam);
		nX = nMx-60 ;//클릭했을때 비행기 제일 왼쪽 위치가 됨.
		nY = nMy - 15;
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		return FALSE; // 커서 움직이는 명령어


	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_CONTROL_START:
			nX = 30, nY = 100;
			nn = 1; //single
			nn2 = 1; //start
			SetTimer(hWnd, max + 2, 3000, NULL); //타이머 11을 3초동안
			SetTimer(hWnd, max + 3, 10, NULL);
			enemy1 = true; // 적이 보임
			gamestart = 1;
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case ID_CONTROL_STOP:
			nn = 0; 
			nn2 = 0; //stop
			nX = 0, nY = 0;
			for (i = 1; i <= max; i++);
			{
				if (nBX[i] == 2)
				{
					KillTimer(hWnd, i);
					//nBw[i] = 1;
				}
			}
			KillTimer(hWnd, max + 2);
			KillTimer(hWnd, max + 3);
			gamestart = 0;

			enemy1 = false;
			enemy2 = false;
			nb = 0; nX = 0; nY = 100;

			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case ID_CONTROL_EXIT:
			PostQuitMessage(0);
			break;
		case ID_BULLET_SINGLE:
			nn = 1;
			break;
		case ID_BULLET_MULTI:
			nn = 2;
			break;
		case ID_ENEMY_ONE:

			ne = 1;
			break;
		case ID_ENEMY_TWO:
			enemy1 = TRUE;
			enemy2 = TRUE;
			ne = 2;
			break;


		}
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_UP:
			if (nn != 0) {
				nY -= 5;
				if (nY < 0) nY = 480;
			}
			break;
		case VK_DOWN:
			if (nn != 0) {
				nY += 5;
				if (nY > 480) nY = 0;
			}
			break;
		case VK_LEFT:
			if (nn != 0) {
				nX -= 5;
				if (nX < 0) nX = 640;
				nXb += 3;
				if (nXb > 640)nXb = 0;
			}
			break;
		case VK_RIGHT:
			if (nn != 0) {
				nX += 5;
				if (nX > 640) nX = 0;
				nXb -= 3;
				if (nXb < 0) nXb = 640;
			}
			break;
		case VK_RETURN:
			if (nn != 0) {
				if (ne == 1) {
					SetTimer(hWnd, max_no_of_bullet + 2, 2000, NULL);
					enemy1 = TRUE;
				}
				else if (ne == 2) {
					SetTimer(hWnd, max_no_of_bullet + 2, 2000, NULL);
					enemy1 = TRUE;
					enemy2 = TRUE;
				}

			}
			break;

		case VK_SPACE:
			if (nn == 1) { //single
				bn = 0;   //1개만 필요하기 때문에 [0]만 사용                                       
				no_of_fire++;
				nBX[bn] = nX + 127;
				nBY[bn] = nY;
				if (!bullet[bn]) {
					SetTimer(hWnd, bn + 1, 5, NULL);
					bullet[bn] = true;
				}
			}
			if (nn == 2) {//multi
				no_of_fire++;
				nBX[bn] = nX + 127;
				nBY[bn] = nY;
				if (!bullet[bn]) {
					SetTimer(hWnd, bn + 1, 5, NULL);
					bullet[bn] = true;
				}
				bn++;
				if (bn > max_no_of_bullet) bn = 0;
			}

			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		return FALSE;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &pS);
		//   hDC = GetDC(hWnd); 수정한 부분
		hMemDC = CreateCompatibleDC(hDC);

		if (enemy1) {
			SelectObject(hMemDC, hBmpEnemy);
			BitBlt(hDC, nX1e, nY1e, 32, 32, hMemDC, 0, 0, SRCCOPY);
		}
		if (enemy2) {
			SelectObject(hMemDC, hBmpEnemy);
			BitBlt(hDC, nX2e, nY2e, 32, 32, hMemDC, 0, 0, SRCCOPY);
		}
		for (int n = 0; n <= max_no_of_bullet; n++) {    // bullet은 10번 실행되야 하므로
			if (bullet[n]) //bullet[n]이 1일 때 (작동)
			{
				SelectObject(hMemDC, hBmpBullet);
				BitBlt(hDC, nBX[n], nBY[n], 32, 30, hMemDC, 0, 0, SRCCOPY);               // Bullet을 윈도우창에 그리는 코드
			}
			if (hit[n])
			{
				SelectObject(hMemDC, hBmpExplosion);
				if (ne == 1) { //적이 1개
					BitBlt(hDC, nX1e, nY1e + 2, 32, 28, hMemDC, 0, 0, SRCCOPY);
					//BitBlt(hDC, nX2e, nY2e + 2, 32, 28, hMemDC, 0, 0, SRCCOPY);
					hit[n] = 0;            // 다시 false로 만들어야함
					Sleep(100);            // 폭파 후 잠깐 멈춰라 그래야 폭파가 계속보이는 오류가 나지않는다
				}
				else if (ne == 2) {
					if (nBX[n] >= nX1e - 15 && nBX[n] <= nX1e + 15 && nBY[n] >= nY1e - 15 && nBY[n] < nY1e + 15)
					{
						BitBlt(hDC, nX1e, nY1e + 2, 32, 28, hMemDC, 0, 0, SRCCOPY);
					}
					else if (nBX[n] >= nX2e - 15 && nBX[n] <= nX2e + 15 && nBY[n] >= nY2e - 15 && nBY[n] < nY2e + 15) {

						BitBlt(hDC, nX2e, nY2e + 2, 32, 28, hMemDC, 0, 0, SRCCOPY);
						hit[n] = 0;            // 다시 false로 만들어야함
						Sleep(100);            // 폭파 후 잠깐 멈춰라 그래야 폭파가 계속보이는 오류가 나지않는다
					}
				}

			}
		}

		//airplane
		SelectObject(hMemDC, hBmpAir);
		BitBlt(hDC, nX, nY, 127, 37, hMemDC, 0, 0, SRCCOPY); //127,37이 비행기 크기

									  //background
		SelectObject(hMemDC, hBmpBack);
		BitBlt(hDC, nXb, 321, 640, 159, hMemDC, 0, 0, SRCCOPY);
		BitBlt(hDC, nXb - 640, 321, 640, 159, hMemDC, 0, 0, SRCCOPY);
		if (nn2 == 1) {
			sprintf_s(szText, "No of fire: %d, No of hit: %d", no_of_fire, no_of_hit);
			TextOut(hDC, 300, 10, szText, lstrlen(szText));
		}
		DeleteDC(hMemDC);
		ReleaseDC(hWnd, hDC);
		EndPaint(hWnd, &pS);
		return FALSE;

	case WM_TIMER:
		hDC = GetDC(hWnd);
		hMemDC = CreateCompatibleDC(hDC);

		for (i = 0; i <= max_no_of_bullet; i++) {            // 여기에 ' = ' 이 빠짐
			if (wParam == i + 1)    //timer no 1
			{
				nBX[i] += 5;
				if (nBX[i] > 640)
				{
					KillTimer(hWnd, i + 1);
					bullet[i] = false;
				}
			}
		}
		if (wParam == max_no_of_bullet + 2)    //timer no 2
		{
			if (ne == 1) {
				nX1e = (rand() % 308) + 300;
				nY1e = (rand() % 230);
			}
			else if (ne == 2) {
				nX1e = (rand() % 308) + 300;
				nY1e = (rand() % 230);
				nX2e = (rand() % 300) + 280;
				nY2e = (rand() % 240);
			}
		}
		for (int n = 0; n <= max_no_of_bullet; n++) {    //적공격
			if (bullet[n]) { //bullet이 1이다.(총알을 쏜다)  그러면 밑에것을 실행
				if (ne == 1) {

					if (nBX[n] >= nX1e - 15 && nBX[n] <= nX1e + 15 && nBY[n] >= nY1e - 15 && nBY[n] < nY1e + 15)
					{
						bullet[n] = false; //0이다(총알 그만 쏨.)
						//enemy = false;    
						KillTimer(hWnd, n + 1);
						//KillTimer(hWnd, 2);  // ->multi에서는 지우기
						hit[n] = TRUE; //hit 켜짐. (터지는 사진 나옴)
						no_of_hit++;


						//MessageBox(hWnd, "Enemy Killed", "Congraturations!", NULL);
					}
				}
				else if (ne == 2) {
					if (nBX[n] >= nX1e - 15 && nBX[n] <= nX1e + 15 && nBY[n] >= nY1e - 15 && nBY[n] < nY1e + 15)
					{
						bullet[n] = false;
						//enemy = false;    
						KillTimer(hWnd, n + 1);
						//KillTimer(hWnd, 2);  // ->multi에서는 지우기
						hit[n] = TRUE;
						no_of_hit++;


						//MessageBox(hWnd, "Enemy Killed", "Congraturations!", NULL);
					}
					if (nBX[n] >= nX2e - 15 && nBX[n] <= nX2e + 15 && nBY[n] >= nY2e - 15 && nBY[n] < nY2e + 15)
					{
						bullet[n] = false;
						//enemy = false;    
						KillTimer(hWnd, n + 1);
						//KillTimer(hWnd, 2);  // ->multi에서는 지우기
						hit[n] = TRUE;
						no_of_hit++;
					}

				}



			}
		}



		DeleteDC(hMemDC);
		ReleaseDC(hWnd, hDC);
		DeleteObject(hMemDC);

		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		return FALSE;


	case WM_DESTROY:
		PostQuitMessage(0);
		return FALSE;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}