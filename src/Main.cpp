
// INCLUDES ///////////////////////////////////////////////

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <vector>
#include <crtdbg.h>


#include "Graphics.h"     // Graphics File


// DEFINES ////////////////////////////////////////////////

// defines for windows 
#define WINDOW_CLASS_NAME "3D Engine "

// MACROS /////////////////////////////////////////////////

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define K_LEFT		VK_LEFT
#define K_RIGHT		VK_RIGHT
#define K_UP		VK_UP
#define K_DOWN		VK_DOWN
#define K_SPACE     VK_SPACE

// GLOBALS ////////////////////////////////////////////////
HWND      main_window_handle = NULL; // globally track main window
HINSTANCE hinstance_app      = NULL; // globally track hinstance



cGraphics  g_Graphics;
cFont      g_Font;
cTexture   g_Menu;
cSprite    g_a;

Camera   TheCamera(Camera::LANDOBJECT);

float x,y,z; 
// FUNCTIONS //////////////////////////////////////////////
LRESULT CALLBACK WindowProc(HWND hwnd, 
	UINT msg, 
	WPARAM wparam, 
	LPARAM lparam)
{
	// this is the main message handler of the system

	// what is the message 
	switch(msg)
	{	
	case WM_CREATE: 
		{
			// do initialization stuff here
			// return success
			return(0);
		} break;
	case WM_LBUTTONDOWN:


		break;

	case WM_PAINT: 
		{
 
			// return success
			return(0);
		} break;

	case WM_DESTROY: 
		{

			// kill the application, this sends a WM_QUIT message 
			PostQuitMessage(0);

			// return success
			return(0);
		} break;

	default:break;

	} // end switch

	// process any messages that we didn't take care of 
	return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc

///////////////////////////////////////////////////////////




int Game_Main(void *parms = NULL, int num_parms = 0)
{
	Start_Clock();

	static DWORD  Timer = timeGetTime();
	unsigned long timeDelta;
	timeDelta= (timeGetTime() - Timer);
	Timer = timeGetTime();
	static int LastAnim = -1; 

	// for now test if user is hitting ESC and send WM_CLOSE
	if (KEYDOWN(VK_ESCAPE))
		SendMessage(main_window_handle,WM_CLOSE,0,0);

	g_Graphics.Clear();


	if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
		TheCamera.walk(0.1f * timeDelta);

	if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
		TheCamera.walk(-0.1f * timeDelta);

	if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
		TheCamera.strafe(-0.1f * timeDelta);

	if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
		TheCamera.strafe(0.1f * timeDelta);


	D3DXMATRIX V;
	TheCamera.getViewMatrix(&V);
	g_Graphics.GetDeviceCOM()->SetTransform(D3DTS_VIEW, &V);

	float xpos = KeyPressed(0, K_RIGHT) - KeyPressed(0, K_LEFT);
	float zpos = KeyPressed(0, K_UP)    - KeyPressed(0, K_DOWN);
 
	g_Graphics.BeginDraw();

	g_Graphics.EnableZBuffer(TRUE);

 
	g_Graphics.EnableZBuffer(FALSE);  
	g_Graphics.EnableAlphaBlending(TRUE);

	g_Menu.Blit(10,350);

	g_Graphics.EndDraw();


	g_Graphics.Draw();



	Wait_Clock(30);



	// return success or failure or your own return code here
	return(1);

} // end Game_Main

////////////////////////////////////////////////////////////

int Game_Init(void *parms = NULL, int num_parms = 0)
{


	g_Graphics.Init3D(main_window_handle,640,480,1);
	g_Graphics.SetPerspective(D3DX_PI/4, 1.3333f, 1.0f, 20000.0f);
 
	D3DXVECTOR3 pos(14.0f, 144.0f, -300.f);
	TheCamera.setPosition(&pos);
	TheCamera.roll(84);
	 

	return(1);
} // end Game_Init

/////////////////////////////////////////////////////////////

int Game_Shutdown(void *parms = NULL, int num_parms = 0)
{

	g_Graphics.Free3D();
 
	return(1);

} // end Game_Shutdown




// WINMAIN ////////////////////////////////////////////////
int WINAPI WinMain(	HINSTANCE hinstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int ncmdshow)
{

	WNDCLASSEX winclass; // this will hold the class we create
	HWND	   hwnd;	 // generic window handle
	MSG		   msg;		 // generic message

	// first fill in the window class stucture
	winclass.cbSize         = sizeof(WNDCLASSEX);
	winclass.style			= CS_DBLCLKS | CS_OWNDC | 
		CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc	= WindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW); 
	winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName	= NULL;
	winclass.lpszClassName	= WINDOW_CLASS_NAME;
	winclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);

	// save hinstance in global
	hinstance_app = hinstance;

	// register the window class
	if (!RegisterClassEx(&winclass))
		return(0);

	// create the window
	if (!(hwnd = CreateWindowEx(NULL,                  // extended style
		WINDOW_CLASS_NAME,     // class
		"RedLine TEAM - Demo Version 1.0 ", // title
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		120,100,	  // initial x,y
		800,600,  // initial width, height
		NULL,	  // handle to parent 
		NULL,	  // handle to menu
		hinstance,// instance of this application
		NULL)))	// extra creation parms
		return(0);

	// save main window handle
	main_window_handle = hwnd;

	// initialize game here
	Game_Init();

	// enter main event loop
	while(TRUE)
	{
		// test if there is a message in queue, if so get it
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{ 
			// test if this is a quit
			if (msg.message == WM_QUIT)
				break;

			// translate any accelerator keys
			TranslateMessage(&msg);

			// send the message to the window proc
			DispatchMessage(&msg);
		} // end if

		// main game processing goes here
		Game_Main();

	} // end while

	// closedown game here
	Game_Shutdown();

	// return to Windows like this
	return(msg.wParam);

} // end WinMain

///////////////////////////////////////////////////////////


float static KeyPressed(float fValue, DWORD dwKEY)
{
	float kSpeed = 3.01f;

	if(GetKeyState(dwKEY) & 0x80) { fValue += kSpeed; }
	return fValue;
}// End of KeyPressed(..)
 