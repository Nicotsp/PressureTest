#pragma once
#include <string>
#include <iostream>
/**
 * Cette classe g�re l'aquisition des test effectu�s sur la tablette.
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
