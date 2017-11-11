/*
============================================================================
Name        :	SPACE GAME
Author      :	Piyathida Phaenghom 
				Panukit Sangsawang
				Pakon Ruchirekserikun
				Panupong Thammachart
				Phanuphonf Salaemat

Version     :	SPACE DEMO
Copyright   :	Your copyright notice
Description :	main.cpp

Credit Music : https://www.youtube.com/watch?v=iuWuzDIhK-o&app=desktop
			   https://www.zapsplat.com/sound-effect-categories/

Credit Source Code : เขียนเกมส์อย่างมืออาชีพด้วย Visual C++ และ  DirectX โดย นิรุธ อำนวยศิลป์ 
============================================================================
*/

#include <windows.h>
#include <windowsx.h>

#include "ddraw.h"
#include "dinput.h" 

#include "mmsystem.h"
#include <iostream>


// ===============|| Control Actor ||====================
#define KEY(buffer,key) (buffer[key] & 0x80)

// =================|| Jet engine ||=====================
#define JETTIME 100
// =================|| shooting ||=====================
#define SHOOTINGTIME 101
// =================|| shooting ||=====================
#define FLYINGTIME 102
// =================|| shooting ||=====================
#define BOOMTIME 103
// ======================================================

BOOL shoot = FALSE;

LPDIRECTDRAW7			dd = NULL;
LPDIRECTDRAWSURFACE7	primary = NULL;
LPDIRECTDRAWSURFACE7	back = NULL;

// ==============|| Background Image ||==================
LPDIRECTDRAWSURFACE7	bmpback = NULL;
int screenx, screeny;
// ===================|| Actor ||========================
LPDIRECTDRAWSURFACE7	actor = NULL;
int actorx, actory;
int actor_width, actor_height;
BOOL actor_stop = TRUE;
// ==================|| Jet engine ||====================
int actor_frame = 1;

// ================|| Control Actor ||===================
char buffer[256];
LPDIRECTINPUT	input;
LPDIRECTINPUTDEVICE		keyboard;

// ==================|| Weapon ||====================
LPDIRECTDRAWSURFACE7	weapon = NULL;
int weaponx, weapony;
int weapon_width, weapon_height;
int weapon_frame = 1;
// ==================|| Monster ||====================
LPDIRECTDRAWSURFACE7 mon=NULL;
int mX, mY;
int mW, mH;
int mFrame = 1;
int mStep = 3;

// ==================|| Score ||====================
int score = 0;
int level = 1;
int ammo = 5;
BOOL gameover = TRUE;
// ==================|| BOOM ||====================
LPDIRECTDRAWSURFACE7 boom = NULL;
int bX, bY;
int bW, bH;
int bFrame=1;

	BOOL mon_die=FALSE ;
	BOOL startboom = FALSE;

// ======================================================

LPDIRECTDRAWSURFACE7 GetBmp(LPDIRECTDRAW7 directdraw, LPCTSTR filename)
{
	HDC hdc;
	HBITMAP bit;
	LPDIRECTDRAWSURFACE7 surf;

	bit = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);

	if (!bit) return NULL;

	BITMAP bitmap;
	GetObject(bit, sizeof(BITMAP), &bitmap);
	int surf_width = bitmap.bmWidth;
	int surf_height = bitmap.bmHeight;

	HRESULT result;
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = surf_width;
	ddsd.dwHeight = surf_height;
	result = directdraw->CreateSurface(&ddsd, &surf, NULL);

	if (result != DD_OK)
	{
		DeleteObject(bit);
		return NULL;
	}
	else
	{
		surf->GetDC(&hdc);
		HDC bit_dc = CreateCompatibleDC(hdc);
		SelectObject(bit_dc, bit);
		BitBlt(hdc, 0, 0, surf_width, surf_height, bit_dc, 0, 0, SRCCOPY);
		surf->ReleaseDC(hdc);
		DeleteObject(bit_dc);
	}
	return surf;
}

RECT GetPic(int howx, int howy, int which, int line)
{
	RECT rtmp;
	rtmp.left = (howx*(which - 1)) + 1;
	rtmp.top = (howy*(line - 1)) + 1;
	rtmp.right = rtmp.left + howx + 1;
	rtmp.bottom = (rtmp.top + howy) - 1;
	return rtmp;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
/*
			case WM_KEYDOWN:
			{
			// =================|| ESC_ExitGame ||===================
			if (wparam == VK_ESCAPE)
			{
			PostMessage(hwnd, WM_DESTROY, 0, 0);
			}
			}break;
*/
// ===================|| Time||=======================
			case WM_TIMER : 
			{
				//actor
				if (wparam == JETTIME)
				{
					actor_frame++;
					if (actor_frame >= 3) actor_frame = 1;
				}
				//shoot
				if ((wparam == SHOOTINGTIME) && (shoot == TRUE))
				{
					weapon_frame++;
					if (weapon_frame > 3) weapon_frame = 1;
				}
				//Monster
				if ((wparam == FLYINGTIME) && (mon_die==FALSE))
				{
					mFrame++;
					if (mFrame >= 3) mFrame = 1;
				}
				//BOOM
				if ((wparam == BOOMTIME) && (mon_die == TRUE))
				{
					bFrame++;
					if (bFrame >= 5)
					{
						mon_die = FALSE;
						startboom = FALSE;
						mX = screenx;
						bFrame = 1;
						weapony = 0;
						mStep += 1;
						level += 1;
					}

				}
			}break;


			case WM_DESTROY :
			{
// ==============|| Background Image ||==================
					bmpback->Release();
					bmpback = NULL;

// ===================|| Actor ||========================
					actor->Release();
					actor = NULL;

// ===============|| Input Control Actor ||====================
					keyboard->Unacquire();
					keyboard->Release();
					keyboard = NULL;

					input->Release();
					input = NULL;

/*-----------------------------Time obj----------------------------------*/
// =================|| Jet engine ||================
					KillTimer(hwnd, JETTIME);
// =================|| shoot ||=====================
					KillTimer(hwnd, SHOOTINGTIME);
// =================|| shoot ||=====================
					KillTimer(hwnd, FLYINGTIME);
// =================|| shoot ||=====================
					KillTimer(hwnd, BOOMTIME);

/*-----------------------------------------------------------------------*/
// =================|| weapon ||=====================
					weapon->Release();
					weapon = NULL;
// =================|| Monster ||====================
					mon->Release();
					mon = NULL;
// =================|| BOOM ||=======================
					boom->Release();
					boom= NULL;
// ======================================================
					
					back->Release();
					back = NULL;
					primary->Release();
					primary = NULL;
					
					dd->Release();
					dd = NULL;
					
					PostQuitMessage(0);
			} break;
	}
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	WNDCLASSEX winclass;
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC | CS_DBLCLKS;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;

	winclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winclass.lpszClassName = "MyWin";
	winclass.lpszMenuName = NULL;
	winclass.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(hinstance, IDI_APPLICATION);
	winclass.hIconSm = LoadIcon(hinstance, IDI_APPLICATION);
	
	RegisterClassEx(&winclass);

	HWND hwnd;
	MSG msg;

	hwnd = CreateWindowEx(WS_EX_TOPMOST, "MyWin", "Win32 Window", WS_POPUP, 0, 0,
		GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		NULL, NULL, hinstance, NULL);

	ShowWindow(hwnd, ncmdshow);
	UpdateWindow(hwnd);

	DirectDrawCreateEx(NULL, (LPVOID*)&dd, IID_IDirectDraw7, NULL);
	dd->SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
	dd->SetDisplayMode(640, 480, 16, 0, 0);

	DDSURFACEDESC2	surf;
	ZeroMemory(&surf, sizeof(surf));
	surf.dwSize = sizeof(surf);
	surf.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	surf.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	surf.dwBackBufferCount = 1;

	dd->CreateSurface(&surf, &primary, NULL);

	DDSCAPS2 ddscaps;
	ZeroMemory(&ddscaps, sizeof(ddscaps));
	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	primary->GetAttachedSurface(&ddscaps,&back);

// =================|| Background Image ||==================
	bmpback = GetBmp(dd, "space.bmp");
	screenx = GetSystemMetrics(SM_CXSCREEN);
	screeny = GetSystemMetrics(SM_CYSCREEN);

	primary->Blt(NULL, bmpback, NULL, DDBLT_WAIT, NULL);
	back->Blt(NULL, bmpback, NULL, DDBLT_WAIT, NULL);


// ======================|| Actor ||========================
	DDCOLORKEY ddck;
	ddck.dwColorSpaceLowValue = ddck.dwColorSpaceHighValue = 0;

	actor = GetBmp(dd, "actor.bmp");
	actor->SetColorKey(DDCKEY_SRCBLT, &ddck);

	actor_width = 116;
	actor_height = 58;

	actorx = (screenx / 2) - (actor_width / 2);
	actory = (screeny - 100);
/*-----------------------------Time-------------------------------------*/
// ===================|| Jet engine ||======================
	SetTimer(hwnd, JETTIME, 100, NULL);

// ===================|| Time Shooting ||===================
	SetTimer(hwnd, SHOOTINGTIME, 700, NULL);

// ===================|| Time Shooting ||===================
	SetTimer(hwnd, FLYINGTIME, 400, NULL);
// ===================|| Time BOOM! ||===================
	SetTimer(hwnd, BOOMTIME, 300, NULL);
/*-----------------------------Time-------------------------------------*/

// ===================|| Monster ||======================
	mon = GetBmp(dd,"Mon01.bmp");
	mon->SetColorKey(DDCKEY_SRCBLT, &ddck);
	mX = screenx;
	mY = 100;
	mW = 116;
	mH = 58;
// ===================|| BOOM ||========================
	boom = GetBmp(dd,"boom.bmp");
	boom->SetColorKey(DDCKEYCAPS_SRCBLT,&ddck);
	bW = 116;
	bH = 58;


	
// =================|| Control Actor ||=====================
	DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&input, NULL);
	
	input->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);

	keyboard->SetDataFormat(&c_dfDIKeyboard);
	keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	keyboard->Acquire();

// =====================|| Weapon ||========================
	weapon = GetBmp(dd, "weapon.bmp");
	weapon_width = 20;
	weapon_height = 29;
	weapon->SetColorKey(DDCKEY_SRCBLT, &ddck);

// ========================||Sound||=================================
	PlaySound(TEXT("A.wav"), NULL, SND_NODEFAULT | SND_ASYNC | SND_LOOP);

// ==================|| Score ||====================
	CHAR str[60];
	HDC hdc;
	hdc = GetDC(hwnd);
// ======================================================
	ShowCursor(FALSE);
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
				return msg.wParam;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
// ========================================|| Game Main ||================================================
		else
		{
			
			// Control Actor
			
			keyboard->GetDeviceState(sizeof(buffer), (LPVOID)&buffer);
				if (keyboard->Acquire() == DI_OK)
				{
					keyboard->GetDeviceState(sizeof(buffer), (LPVOID)&buffer);
				}

				if (KEY(buffer, DIK_LEFT))
				{
					actorx -= 5;
					if (actorx < 0) actorx = 1;
				}
				
				if (KEY(buffer, DIK_RIGHT))
				{
					actorx += 5;
					if (actorx >(screenx - actor_width)) actorx = screenx - actor_width;
				}	

			//start game
			if (KEY(buffer, DIK_N) && (gameover == TRUE))
			{
				bmpback = GetBmp(dd, "space.bmp");

				ammo = 5;
				level = 1;
				score = 0;
				mon_die = FALSE;
				startboom = FALSE;
				mX = screenx;
				bFrame = 1;
				weapony = 0;
				mStep = 5;
				gameover = FALSE;
				
			}

			if (gameover == TRUE)
			{
				bmpback = GetBmp(dd, "LOGO.bmp");
				mon_die = TRUE;
				PlaySound(TEXT("A.wav"), NULL, SND_NODEFAULT | SND_ASYNC | SND_LOOP | SND_NOSTOP);
			}


			//Exit Game
			if (KEY(buffer, DIK_ESCAPE))
			{
				PostMessage(hwnd, WM_DESTROY, 0, 0);
			}

			//main Func

			if (gameover == FALSE)
			{
				if (KEY(buffer, DIK_SPACE) && (shoot == FALSE))
				{
					weaponx = actorx + ((actor_width / 2) - (weapon_width / 2)) - 1.5;
					weapony = actory;
					shoot = TRUE;
					ammo--;
					PlaySound(TEXT("A.wav"), NULL, SND_NODEFAULT | SND_ASYNC | SND_LOOP | SND_NOSTOP);
					PlaySound(TEXT("shoot.wav"), NULL, SND_NODEFAULT | SND_ASYNC | SND_LOOP);
					PlaySound(TEXT("A.wav"), NULL, SND_NODEFAULT | SND_ASYNC | SND_LOOP);
				}
				// <Space Bar> Shooting
				
				if ((shoot == TRUE) && (startboom == FALSE))

				{
					weapony -= 5;
					if (weapony < 0) shoot = FALSE;

					RECT r = GetPic(weapon_width, weapon_height, weapon_frame, 1);
					back->BltFast(weaponx, weapony, weapon, &r, DDBLTFAST_SRCCOLORKEY);

					if ((ammo == 0) && weapony <= 0) //เช็คกระสุน เเละ gameover
					{
						back->GetDC(&hdc);
						SetBkColor(hdc, RGB(0, 0, 0));
						SetBkMode(hdc, TRANSPARENT);
						SetTextColor(hdc, RGB(255, 255, 255));
						TextOut(hdc, 50, 100, "Game Over! N to continue , ESC to quit", 15);
						back->ReleaseDC(hdc);
						gameover = TRUE;

					}

					
								if ((weaponx > mX) && (weaponx <= (mX + mW)))
									{
										if ((weapony > mY) && (weapony <= (mY + mH)))
										{
										//Boom!
										bX = mX;
										bY = mY;
										mon_die = TRUE;
										startboom = TRUE;
										score += 100;
										ammo += 2;
										PlaySound(TEXT("Bomb.wav"), NULL, SND_NODEFAULT | SND_ASYNC | SND_LOOP);
										PlaySound(TEXT("A.wav"), NULL, SND_NODEFAULT | SND_ASYNC | SND_LOOP);
										}

									}
				}

				//monster
				if (mon_die == FALSE)
				{
					mX -= mStep;
					if (mX < (-mW)) mX = screenx;
				}
				RECT mRect = GetPic(mW, mH, mFrame, 1);
				back->BltFast(mX, mY, mon, &mRect, DDBLTFAST_SRCCOLORKEY);

				//BOOM
				if (startboom == TRUE)
				{
					RECT bRect = GetPic(bW, bH, bFrame, 1);
					back->BltFast(bX, bY, boom, &bRect, DDBLTFAST_SRCCOLORKEY);
				}

				//Actor
				RECT r = GetPic(actor_width, actor_height, actor_frame, 1);
				back->BltFast(actorx, actory, actor, &r, DDBLTFAST_SRCCOLORKEY);

				//score  
				back->GetDC(&hdc);
				SetBkColor(hdc, RGB(0, 0, 0));
				SetBkMode(hdc, TRANSPARENT);
				SetTextColor(hdc, RGB(255, 255, 255));
				wsprintf(str, "Score:%d", score);
				TextOut(hdc, 530, 10, str, strlen(str));
				wsprintf(str, "Ammo:%d", ammo);
				TextOut(hdc, 530, 30, str, strlen(str));
				wsprintf(str, "Level:%d", level);
				TextOut(hdc, 530, 50, str, strlen(str));

				back->ReleaseDC(hdc);
					

				
			}
				//Flip
				HRESULT hret;
				hret = primary->Flip(NULL, DDFLIP_WAIT);
				if (hret == DD_OK)
				{
					if (back->Restore() == DD_OK)
						back->Blt(NULL, bmpback, NULL, DDBLT_WAIT, NULL);
				}
				
			}
		



// =================================================================================
	}
	ShowCursor(TRUE);
	return (msg.wParam);
}