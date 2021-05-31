///////////////////////////////////////////////////////////////////////////////
//
//	DESCRIPTION
//		This demo shows how to use Wintab to detect/display pen pressure input.
//
//	COPYRIGHT
//		Copyright (C) 1998  LCS/Telegraphics
//		Copyright (c) 2014-2020 Wacom Co., Ltd.
//		All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

#include "MSGPACK.h"
#include "WINTAB.h"
#define PACKETDATA	(PK_X | PK_Y | PK_BUTTONS | PK_NORMAL_PRESSURE | PK_TANGENT_PRESSURE | PK_ORIENTATION | PK_ROTATION | PK_TIME)
#define PACKETMODE	PK_BUTTONS
#include "PKTDEF.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include "PressureTest.h"

constexpr int MAX_LOADSTRING = 100;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Global Variables:

HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];			// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];	// the main window class name

char* gpszProgramName = "PressureTest";
static LOGCONTEXT glogContext = { 0 };

FILE* fp;

bool saving=false;

string name;
string firstname;
string exercice;
int trial;


//////////////////////////////////////////////////////////////////////////////
// Forward declarations of functions included in this code module:
ATOM					MyRegisterClass(HINSTANCE);
BOOL					InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	PatInfo(HWND, UINT, WPARAM, LPARAM);
HCTX static NEAR TabletInit(HWND hWnd);
void Cleanup(void);

////////////////////////////////////////////////////////////////////////////////
int APIENTRY _tWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PRESSURETEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PRESSURETEST));
	
	fp = fopen("patient.txt", "w+");

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	

	// Return Wintab resources.
	Cleanup();
	

	return static_cast<int>(msg.wParam);
}

//////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style				= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc		= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance			= hInstance;
	wcex.hIcon				= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PRESSURETEST));
	wcex.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE + 1);
	wcex.lpszMenuName		= MAKEINTRESOURCE(IDC_PRESSURETEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm			= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION: InitInstance(HINSTANCE, int)
//
//  PURPOSE: Saves instance handle and creates main window
//
//  COMMENTS:
//
//      In this function, we save the instance handle in a global variable and
//      create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	if (!LoadWintab())
	{
		ShowError("Wintab not available");
		return FALSE;
	}

	/* check if WinTab available. */
	if (!gpWTInfoA(0, 0, NULL))
	{
		ShowError("WinTab Services Not Available.");
		return FALSE;
	}

	HWND hWnd = CreateWindow(
		szWindowClass, szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	HWND lol = CreateWindowExA(0, "WindowOfDLL", szTitle, WS_SIZEBOX | WS_CLIPSIBLINGS, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, hInstance, NULL);
	SetParent(lol, hWnd);
	ShowWindow(lol, SW_SHOWNORMAL);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;

	static HCTX hCtx = NULL;
	static POINT ptOld, ptNew;
	static UINT prsOld, prsNew;
	static UINT max_pressure;
	static UINT half_axis;
	static int azimuth;
	static int altitude;
	static UINT rotation;
	static RECT rcClient;
	PAINTSTRUCT psPaint;
	
	PACKET pkt;
	BOOL fHandled = TRUE;
	LRESULT lResult = 0L;
	static int xMousePos = 0;
	static int yMousePos = 0;
	static int t = 0;

	switch (message)
	{
	case WM_CREATE:
		hCtx = TabletInit(hWnd);
		if (!hCtx)
		{
			ShowError("Could Not Open Tablet Context.");
			SendMessage(hWnd, WM_DESTROY, 0, 0L);
		}
		break;

	case WM_MOVE:
	case WM_SIZE:
		{
			AXIS tabletPressure = { 0 };
			gpWTInfoA(WTI_DEVICES, DVC_NPRESSURE, &tabletPressure);
			max_pressure = tabletPressure.axMax + 1;

			GetClientRect(hWnd, &rcClient);
			// shorter half-axis <--> max_pressure
			half_axis = min(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top) / 2;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
			
		case IDM_RECORDING:
			//PARTIE 1 :
			//pop up pour demander info des patients
			//renseigne patNum (numero patient) et exerciseName (nom de l'exercice)
		
			//addPatientData();   //se r�f�rer Utils.cpp ligne 50
			
			//PARTIE 2:
			//sauvergarder patient.txt dans autre fichier, nomm� ce fichier avec infos pr�c�dentes 
			//Save(patNum, exerciseName); //se r�f�rer Utils.cpp ligne 44
			if (saving == true) {
				int msgboxID = MessageBox(
					NULL,
					"Recording will end.\nAre you sure ?",
					"Warning",
					MB_ICONEXCLAMATION | MB_YESNO);

				if (msgboxID == IDYES)
				{
					saving = !saving;
					
					string line;
					ifstream ini_file{ "patient.txt" };
					ofstream out_file{ "copy.txt" };
					out_file << "X Y Pression Azimuth Altitude\n";
					int i = 1;
					while (getline(ini_file, line) && i < line_count("patient.txt")) {
						out_file << line<<"\n";
						i += 1;
					}
				}
			}
			else if (saving==false){
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, PatInfo);

				int msgboxID = MessageBox(
					NULL,
					"Recording will start.\nAre you ready ?",
					"Warning",
					MB_ICONEXCLAMATION | MB_YESNO);

				if (msgboxID == IDYES)
				{
					saving = !saving;
				}
			}
			

			break;
		case IDM_EXIT:
			fclose(fp);
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
		if (hdc = BeginPaint(hWnd, &psPaint))
		{
			POINT scrPoint = { ptNew.x, ptNew.y };
			ScreenToClient(hWnd, &scrPoint);

			RECT clientRect;
			GetClientRect(hWnd, &clientRect);

			// Draw horizontal line of cross.
			PatBlt(hdc,
				clientRect.left,      scrPoint.y,
				clientRect.right - 1, 1,
				DSTINVERT);

			// Draw vertical line of cross.
			PatBlt(hdc,
				scrPoint.x, clientRect.top,
				1,          clientRect.bottom - 1,
				DSTINVERT);

			// scale the input pressure from max_pressure to
			// the smaller dimension of the display window
			LONG width = prsNew * half_axis / max_pressure;

			// Draw ellipse at cross intersection.
			Ellipse(hdc,
				scrPoint.x - width, scrPoint.y - width,
				scrPoint.x + width, scrPoint.y + width);

			if (prsNew != 0)
			{
				UINT ta_initial = GetTextAlign(hdc);

				// centers string on vertical line of cross
				SetTextAlign(hdc, TA_CENTER);

				// create string from pressure
				std::string s = std::to_string(prsNew);
				const char* p = s.c_str();
				const size_t c = strlen(p);

				SIZE sizl;
				GetTextExtentPoint32(hdc, p, c, &sizl);

				// centers string horizontally on vertical line of cross
				if (scrPoint.x < (clientRect.left + 2 + sizl.cx / 2))
				{
					// don't get too close to the left
					scrPoint.x = clientRect.left + 2 + sizl.cx / 2;
				}
				else if ((clientRect.right - 2 - sizl.cx / 2) < scrPoint.x)
				{
					// don't get too close to the right
					scrPoint.x = clientRect.right - 2 - sizl.cx / 2;
				}
				assert((clientRect.left <= scrPoint.x) & (scrPoint.x <= clientRect.right));

				// centers string vertically on horizontal line of cross
				scrPoint.y -= sizl.cy / 2;
				if (scrPoint.y < clientRect.top)
				{
					// but not too close to top
					scrPoint.y = clientRect.top;
				}
				else if ((clientRect.bottom - sizl.cy) < scrPoint.y)
				{
					// and not too close to bottom
					scrPoint.y = clientRect.bottom - sizl.cy;
				}
				assert((clientRect.top <= scrPoint.y) & (scrPoint.y <= clientRect.bottom));

				TextOut(hdc, scrPoint.x, scrPoint.y, p, c);
				SetTextAlign(hdc, ta_initial);
			}
			EndPaint(hWnd, &psPaint);
		}
		break;

	case WM_DESTROY:
		if (hCtx)
		{
			gpWTClose(hCtx);
		}
		PostQuitMessage(0);
		break;

	case WM_MOUSEMOVE:
		xMousePos = GET_X_LPARAM(lParam);
		yMousePos = GET_Y_LPARAM(lParam);

	case WT_PACKET:
		if (gpWTPacket((HCTX)lParam, wParam, &pkt))
		{
			if (HIWORD(pkt.pkButtons) == TBN_DOWN)
			{
				MessageBeep(0);
			}
			ptOld = ptNew;
			prsOld = prsNew;

			t = pkt.pkTime;

			ptNew.x = pkt.pkX;
			ptNew.y = pkt.pkY;

			prsNew = pkt.pkNormalPressure;
			azimuth = pkt.pkOrientation.orAzimuth;
			altitude = pkt.pkOrientation.orAltitude;


			if (  (ptNew.x != ptOld.x)
				|| (ptNew.y != ptOld.y)
				|| (prsNew != prsOld))
			{
				if (saving == true) {
					InvalidateRect(hWnd, NULL, TRUE);
					//Ligne � lire en-dessous
					fprintf(fp, "%d %i %i %u %d %d\n", t, ptNew.x, ptNew.y, prsNew, azimuth, altitude);
				}
			}

		}

		break;
		


	case WM_ACTIVATE:
		if (GET_WM_ACTIVATE_STATE(wParam, lParam))
		{
			InvalidateRect(hWnd, NULL, TRUE);
		}

		/* if switching in the middle, disable the region */
		if (hCtx)
		{
			gpWTEnable(hCtx, GET_WM_ACTIVATE_STATE(wParam, lParam));
			if (hCtx && GET_WM_ACTIVATE_STATE(wParam, lParam))
			{
				gpWTOverlap(hCtx, TRUE);
			}
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////

INT_PTR CALLBACK PatInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR lpszPassword[16];
	WORD cchPassword;

	switch (message)
	{
	case WM_INITDIALOG:
		// Set password character to a plus sign (+) 
		/*SendDlgItemMessage(hDlg,
			IDC_EDIT_NAME,
			EM_SETPASSWORDCHAR,
			(WPARAM)'+',
			(LPARAM)0);
		*/

		// Set the default push button to "Cancel." 
		SendMessage(hDlg,
			DM_SETDEFID,
			(WPARAM)IDCANCEL,
			(LPARAM)0);

		return TRUE;

	case WM_COMMAND:
		// Set the default push button to "OK" when the user enters text. 
		if (HIWORD(wParam) == EN_CHANGE &&
			LOWORD(wParam) == IDC_EDIT_NAME)
		{
			SendMessage(hDlg,
				DM_SETDEFID,
				(WPARAM)IDOK,
				(LPARAM)0);
		}
		switch (wParam)
		{
		case IDOK:
			// Get number of characters. 
			cchPassword = (WORD)SendDlgItemMessage(hDlg,
				IDC_EDIT_NAME,
				EM_LINELENGTH,
				(WPARAM)0,
				(LPARAM)0);
			if (cchPassword >= 16)
			{
				MessageBox(hDlg,
					"Too many characters.",
					"Error",
					MB_OK);

				EndDialog(hDlg, TRUE);
				return FALSE;
			}
			else if (cchPassword == 0)
			{
				MessageBox(hDlg,
					"No characters entered.",
					"Error",
					MB_OK);

				EndDialog(hDlg, TRUE);
				return FALSE;
			}

			// Put the number of characters into first word of buffer. 
			*((LPWORD)lpszPassword) = cchPassword;

			// Get the characters. 
			SendDlgItemMessage(hDlg,
				IDC_EDIT_NAME,
				EM_GETLINE,
				(WPARAM)0,       // line 0 
				(LPARAM)lpszPassword);

			// Null-terminate the string. 
			lpszPassword[cchPassword] = 0;
			MessageBox(hDlg,
				lpszPassword,
				"Did it work?",
				MB_OK);
			name = lpszPassword;
			FILE* fo;
			fo = fopen("patientinfo.txt", "w+");
			fprintf(fo, "%s \n",name);
			// Call a local password-parsing function. 
			//ParsePassword(lpszPassword);

			EndDialog(hDlg, TRUE);
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return TRUE;
		}
		return 0;
	}
	return FALSE;

	UNREFERENCED_PARAMETER(lParam);
}

//////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////

HCTX static NEAR TabletInit(HWND hWnd)
{
	HCTX hctx = NULL;
	UINT wDevice = 0;
	UINT wExtX = 0;
	UINT wExtY = 0;
	UINT wWTInfoRetVal = 0;
	AXIS TabletX = { 0 };
	AXIS TabletY = { 0 };

	// Set option to move system cursor before getting default system context.
	glogContext.lcOptions |= CXO_SYSTEM;

	// Open default system context so that we can get tablet data
	// in screen coordinates (not tablet coordinates).
	wWTInfoRetVal = gpWTInfoA(WTI_DEFSYSCTX, 0, &glogContext);
	assert(wWTInfoRetVal == sizeof(LOGCONTEXT));

	assert(glogContext.lcOptions & CXO_SYSTEM);

	// modify the digitizing region
	wsprintf(glogContext.lcName, "PrsTest Digitizing %p", hInst);

	// We process WT_PACKET (CXO_MESSAGES) messages.
	glogContext.lcOptions |= CXO_MESSAGES;

	// What data items we want to be included in the tablet packets
	glogContext.lcPktData = PACKETDATA;

	// Which packet items should show change in value since the last
	// packet (referred to as 'relative' data) and which items
	// should be 'absolute'.
	glogContext.lcPktMode = PACKETMODE;

	// This bitfield determines whether or not this context will receive
	// a packet when a value for each packet field changes.  This is not
	// supported by the Intuos Wintab.  Your context will always receive
	// packets, even if there has been no change in the data.
	glogContext.lcMoveMask = PACKETDATA;

	// Which buttons events will be handled by this context.  lcBtnMask
	// is a bitfield with one bit per button.
	glogContext.lcBtnUpMask = glogContext.lcBtnDnMask;

	// Set the entire tablet as active
	// Note: only works with 0th tablet! clear your tablet prefs;
	//       otherwise, you may get some funky behavior
	wWTInfoRetVal = gpWTInfoA(WTI_DEVICES + 0, DVC_X, &TabletX);
	assert(wWTInfoRetVal == sizeof(AXIS));

	wWTInfoRetVal = gpWTInfoA(WTI_DEVICES, DVC_Y, &TabletY);
	assert(wWTInfoRetVal == sizeof(AXIS));

	glogContext.lcInOrgX = 0;
	glogContext.lcInOrgY = 0;
	glogContext.lcInExtX = TabletX.axMax;
	glogContext.lcInExtY = TabletY.axMax;

	// Guarantee the output coordinate space to be in screen coordinates.
	glogContext.lcOutOrgX = GetSystemMetrics(SM_XVIRTUALSCREEN);
	glogContext.lcOutOrgY = GetSystemMetrics(SM_YVIRTUALSCREEN);
	glogContext.lcOutExtX = GetSystemMetrics(SM_CXVIRTUALSCREEN); //SM_CXSCREEN);

	// In Wintab, the tablet origin is lower left.  Move origin to upper left
	// so that it coincides with screen origin.
	glogContext.lcOutExtY = -GetSystemMetrics(SM_CYVIRTUALSCREEN);	//SM_CYSCREEN);

	// Leave the system origin and extents as received:
	// lcSysOrgX, lcSysOrgY, lcSysExtX, lcSysExtY

	// open the region
	// The Wintab spec says we must open the context disabled if we are
	// using cursor masks.
	hctx = gpWTOpenA(hWnd, &glogContext, FALSE);

	return hctx;
}

///////////////////////////////////////////////////////////////////////////////

void Cleanup(void)
{
	UnloadWintab();
}

///////////////////////////////////////////////////////////////////////////////

int line_count(string a)
{
	int count = 0;
	string line;

	/* Creating input filestream */
	ifstream file(a);
	while (getline(file, line))
		count++;

	return count;
}
//////////////////////////////////////////////////////////////////////////////
