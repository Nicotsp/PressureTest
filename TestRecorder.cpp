#include "TestRecorder.h"
#include <WinUser.h>
#include "WacomRecorder.h"
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>
#include <msclr\marshal_atl.h>
#include <msclr\marshal.h>
#include <stdlib.h>
#include <string.h>


/* Gestion de l'appli*/

void TestRecorder::StartRecording() {
	if (FindWindow(L"WacomRecorderWClass", L"Wacom Tablet Recorder")) return;

	TestRecorder::launchWacomRecording(NO_TIME_LIMIT_WACOM);
}

void TestRecorder::StopRecording() {
	if (!FindWindow(L"WacomRecorderWClass", L"Wacom Tablet Recorder")) return;

	killWacomRecording();
	/*createRecordResultsFile();*/
}


/*Gestion de la tablette*/

void TestRecorder::waitingWacomRecorder() {
	while (FindWindow(L"WacomRecorderWClass", L"Wacom Tablet Recorder")) Sleep(100);
}

void TestRecorder::launchWacomRecording(int RecordTime) {

	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	LPSTR lpCmdLine;
	HWND thisWindow;

	if (!FindWindow(L"WacomRecorderWClass", L"Wacom Tablet Recorder")) {
		thisWindow = GetActiveWindow();
		SetWindowPos(thisWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE);
		TabletLaunch(hInstance, hPrevInstance, lpCmdLine, RecordTime);
	}
}

void TestRecorder::killWacomRecording() {

	HWND hWnd;
	hWnd = FindWindow(L"WacomRecorderWClass", L"Wacom Tablet Recorder");
	SendMessage(hWnd, WM_DESTROY, 0, 0L);
	DestroyWindow(hWnd);

}

/*
sting TestRecorder::getPathToDestinationFile() {
	 Duy 

	String^ pathDestiny;

	return pathDestiny /* insérer chemin de sauvegarde des données
} */



