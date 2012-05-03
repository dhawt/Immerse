#ifndef _DEMO_HPP
#define _DEMO_HPP

// Includes:
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <time.h>

// Libraries:
//#include "H:\Lib\Immerse\Code\Core\IMR_Interface.hpp"
#include "C:\Code\Engines\Lib\Immerse\Code\GeomMngr\IMR_GM_Interface.hpp"
#include "C:\Code\Engines\Lib\WinLayer\WinLayer.hpp"
#include "C:\Code\Engines\Lib\GfxLib\GfxLib.hpp"

// Prototypes:
void Cleanup(void);
void InitWindowApp(void);
LRESULT CALLBACK WndProc(HWND hWnd, unsigned int iMessage, 
                         unsigned int wParam, LONG lParam);

#endif
