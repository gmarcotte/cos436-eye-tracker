#ifndef RECOGNIZER_CLASS
#define RECOGNIZER_CLASS


#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;
#using <System.Windows.Forms.dll>
#using <System.dll>

using namespace System;
using namespace System::Windows::Forms;
#include "Eye.h"

#include "EyeTracker/config.h"
//#define MAF_LENGTH = 4 //3 stored, 1 averaged
//#define MAF_DIVISOR = 6//3+2+1


class Recognizer
{
	public:
	enum eyeActions {CENTER, LEFT, RIGHT, UP, DOWN,
                 LEFT_CLOSED, RIGHT_CLOSED, BOTH_CLOSED};
    enum browActions {RAISED, NORMAL}; 

	enum MAF_CONST {MAF_LENGTH = 4,//3 stored, 1 averaged
				    MAF_DIVISOR = 6,//3+2+1
					ACTION_LIMIT = 3,
					BLINK_LIMIT = 12
					};

	//debugging text files
	ofstream of_CalibrationL;
	ofstream of_CalibrationR;
	//System::IO::StreamWriter s;



	//CALIBRATION VARIABLES
	int calibrationPupilTopL;
	int calibrationPupilBottomL;
	int calibrationPupilLeftL;
	int calibrationPupilRightL;
	int calibrationBrowL;

	int calibrationCountPupilTopL;
	int calibrationCountPupilBottomL;
	int calibrationCountPupilLeftL;
	int calibrationCountPupilRightL;
	int calibrationCountBrowL;

	//CALIBRATION VARIABLES
	int calibrationPupilTopR;
	int calibrationPupilBottomR;
	int calibrationPupilLeftR;
	int calibrationPupilRightR;
	int calibrationBrowR;

	int calibrationCountPupilTopR;
	int calibrationCountPupilBottomR;
	int calibrationCountPupilLeftR;
	int calibrationCountPupilRightR;
	int calibrationCountBrowR;
	//moving average filter of the current eye position
	//array[0] is the current, averaged position
	//array[1] to array[MAF_LENGTH] stores the previous (MAF_LENGTH-1) values
	
	//MAF VARIABLES
	//left ones
	int* currentGazePositionLX;
	int* currentGazePositionLY;
	int* currentEyePositionLX;
	int* currentEyePositionLY;
	int* currentEyeSizeLX;
	int* currentEyeSizeLY;
	int* currentPupilRadiusL;
	//right ones
	int* currentGazePositionRX;
	int* currentGazePositionRY;
	int* currentEyePositionRX;
	int* currentEyePositionRY;
	int* currentEyeSizeRX;
	int* currentEyeSizeRY;
	int* currentPupilRadiusR;

	int* browPositionL;
	int* browPositionR;


	//STATE VARIABLES
	int currentLeftEyeAction;
	int currentLeftEyeActionCount;
	int currentRightEyeAction;
	int currentRightEyeActionCount;
	int currentBrowAction;
	int blinkFrameCountLeft;
	int blinkFrameCountRight;
	//when a new eye is given, this updates the stored values in the MAF
	//and computes the new average position
	void updateCurrentLeftEyePosition(Eye leftEye);
	void updateCurrentRightEyePosition(Eye rightEye);
	int updateState(Eye newEyeLeft, Eye newEyeRight);
	int calculateCurrentLeftEyePosition();
	int calculateCurrentRightEyePosition();
	void output(int currentEyeAction);//send the key for a specific action
	void printInfo();
	void updateLeftCalibration(Eye sourceEye, int calibrationType);
	void updateRightCalibration(Eye sourceEye, int calibrationType);
	
	void updateLeftCalibration(Eye sourceEye, int calibrationType, bool resetCalibration);
	void updateRightCalibration(Eye sourceEye, int calibrationType, bool resetCalibration);
	
	//the constructor
	Recognizer();
	~Recognizer();
	

};

#endif