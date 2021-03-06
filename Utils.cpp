///////////////////////////////////////////////////////////////////////////////
//
//	DESCRIPTION
//		Some general-purpose functions for the WinTab demos.
//
//	COPYRIGHT
//		Copyright (c) 2014-2020 Wacom Co., Ltd.
//		All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Utils.h"
#include "winuser.h"

//////////////////////////////////////////////////////////////////////////////

HINSTANCE         ghWintab            = nullptr;

WTINFOA           gpWTInfoA           = nullptr;
WTOPENA           gpWTOpenA           = nullptr;
WTGETA            gpWTGetA            = nullptr;
WTSETA            gpWTSetA            = nullptr;
WTCLOSE           gpWTClose           = nullptr;
WTPACKET          gpWTPacket          = nullptr;
WTENABLE          gpWTEnable          = nullptr;
WTOVERLAP         gpWTOverlap         = nullptr;
WTSAVE            gpWTSave            = nullptr;
WTCONFIG          gpWTConfig          = nullptr;
WTRESTORE         gpWTRestore         = nullptr;
WTEXTSET          gpWTExtSet          = nullptr;
WTEXTGET          gpWTExtGet          = nullptr;
WTQUEUESIZESET    gpWTQueueSizeSet    = nullptr;
WTDATAPEEK        gpWTDataPeek        = nullptr;
WTPACKETSGET      gpWTPacketsGet      = nullptr;
WTMGROPEN         gpWTMgrOpen         = nullptr;
WTMGRCLOSE        gpWTMgrClose        = nullptr;
WTMGRDEFCONTEXT   gpWTMgrDefContext   = nullptr;
WTMGRDEFCONTEXTEX gpWTMgrDefContextEx = nullptr;

// TODO - add more wintab32 function pointers as needed

void save(int patNum, std::string exerciseName) {

	//AJOUTER CODE ICI (copier patient.txt dans autre fichier, le modifier (enlever derniere ligne, nommer nouveau fichier, ajouter colonne avec le temps (freq de 125hz), puis d?truire patient.txt)
	//nom du nouveau fichier -->  patNum_exerciceName_nbEssais.txt    o? nbEssais est incr?menter en fonction du nombre d'essais deja existants, 
}

void addPatientData(HINSTANCE hInstance2) {

	//AJOUTER CODE ICI (pop-up demander le num?ro du patient et le nom de l'exercice)
	//pourra demander plus d'informations par la suite
	
	static TCHAR szWindowClass[] = _T("DesktopApp");
	static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 100,
		NULL,
		NULL,
		hInstance2,
		NULL
	);
	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

	}

}


//////////////////////////////////////////////////////////////////////////////
// Purpose
//		Find wintab32.dll and load it.
//		Find the exported functions we need from it.
//
//	Returns
//		TRUE on success.
//		FALSE on failure.
//
BOOL LoadWintab(void)
{
	// load the wintab32 dll
	ghWintab = LoadLibraryA("Wintab32.dll");

	if (!ghWintab)
	{
		const DWORD err = GetLastError();
		ShowError("Could not load Wintab32.dll: " + std::to_string(err));
		return FALSE;
	}
	// Explicitly find the exported Wintab functions in which we are interested.
	// We are using the ASCII, not unicode versions (where applicable).
	gpWTOpenA           = (WTOPENA)GetProcAddress(ghWintab, "WTOpenA");
	gpWTInfoA           = (WTINFOA)GetProcAddress(ghWintab, "WTInfoA");
	gpWTGetA            = (WTGETA)GetProcAddress(ghWintab, "WTGetA");
	gpWTSetA            = (WTSETA)GetProcAddress(ghWintab, "WTSetA");
	gpWTPacket          = (WTPACKET)GetProcAddress(ghWintab, "WTPacket");
	gpWTClose           = (WTCLOSE)GetProcAddress(ghWintab, "WTClose");
	gpWTEnable          = (WTENABLE)GetProcAddress(ghWintab, "WTEnable");
	gpWTOverlap         = (WTOVERLAP)GetProcAddress(ghWintab, "WTOverlap");
	gpWTSave            = (WTSAVE)GetProcAddress(ghWintab, "WTSave");
	gpWTConfig          = (WTCONFIG)GetProcAddress(ghWintab, "WTConfig");
	gpWTRestore         = (WTRESTORE)GetProcAddress(ghWintab, "WTRestore");
	gpWTExtSet          = (WTEXTSET)GetProcAddress(ghWintab, "WTExtSet");
	gpWTExtGet          = (WTEXTGET)GetProcAddress(ghWintab, "WTExtGet");
	gpWTQueueSizeSet    = (WTQUEUESIZESET)GetProcAddress(ghWintab, "WTQueueSizeSet");
	gpWTDataPeek        = (WTDATAPEEK)GetProcAddress(ghWintab, "WTDataPeek");
	gpWTPacketsGet      = (WTPACKETSGET)GetProcAddress(ghWintab, "WTPacketsGet");
	gpWTMgrOpen         = (WTMGROPEN)GetProcAddress(ghWintab, "WTMgrOpen");
	gpWTMgrClose        = (WTMGRCLOSE)GetProcAddress(ghWintab, "WTMgrClose");
	gpWTMgrDefContext   = (WTMGRDEFCONTEXT)GetProcAddress(ghWintab, "WTMgrDefContext");
	gpWTMgrDefContextEx = (WTMGRDEFCONTEXTEX)GetProcAddress(ghWintab, "WTMgrDefContextEx");

	// TODO - don't forget to NULL out pointers in UnloadWintab().
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// Purpose
//		Uninitializes use of wintab32.dll
//
// Returns
//		Nothing.
//
void UnloadWintab(void)
{
	if (ghWintab)
	{
		FreeLibrary(ghWintab);
		ghWintab = nullptr;
	}
	gpWTOpenA				= nullptr;
	gpWTClose				= nullptr;
	gpWTInfoA				= nullptr;
	gpWTPacket				= nullptr;
	gpWTEnable				= nullptr;
	gpWTOverlap				= nullptr;
	gpWTSave					= nullptr;
	gpWTConfig				= nullptr;
	gpWTGetA					= nullptr;
	gpWTSetA					= nullptr;
	gpWTRestore				= nullptr;
	gpWTExtSet				= nullptr;
	gpWTExtGet				= nullptr;
	gpWTQueueSizeSet		= nullptr;
	gpWTDataPeek			= nullptr;
	gpWTPacketsGet			= nullptr;
	gpWTMgrOpen				= nullptr;
	gpWTMgrClose			= nullptr;
	gpWTMgrDefContext		= nullptr;
	gpWTMgrDefContextEx	= nullptr;
}

//////////////////////////////////////////////////////////////////////////////
// Purpose
//		Display error to user.
//
void ShowError(const std::string &pszErrorMessage_I)
{
	MessageBoxA(NULL, pszErrorMessage_I.c_str(), "Error", MB_OK | MB_ICONERROR);
}

//////////////////////////////////////////////////////////////////////////////

