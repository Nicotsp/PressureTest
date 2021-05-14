#pragma once
#include <string>
#include <iostream>
/**
 * Cette classe gère l'aquisition des test effectués sur la tablette.
 *
 * @author Mansuy & Nguyen
 */

#include <fstream>
#include <msclr\marshal.h>




using namespace std;

class TestRecorder
{
public:
	void StartRecording();
	void StopRecording();
	void waitingWacomRecorder();
	void launchWacomRecording(int RecordTime);
	void killWacomRecording();
	/*string getPathToDestinationFile();*/
};
