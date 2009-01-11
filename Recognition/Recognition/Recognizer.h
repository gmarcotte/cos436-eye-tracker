#ifndef RECOGNIZER_CLASS
#define RECOGNIZER_CLASS


#include <stdlib.h>
#include <stdio.h>
#using <System.Windows.Forms.dll>
#using <System.dll>

using namespace System;
using namespace System::Windows::Forms;
#include "Eye.h"
//#define MAF_LENGTH = 4 //3 stored, 1 averaged
//#define MAF_DIVISOR = 6//3+2+1


class Recognizer
{
	enum eyeActions {CENTER, LEFT, RIGHT, UP, DOWN,
                 LEFT_CLOSED, RIGHT_CLOSED, BOTH_CLOSED};
    enum browActions {RAISED, NORMAL}; 

	enum MAF_CONST {MAF_LENGTH = 4,//3 stored, 1 averaged
				    MAF_DIVISOR = 6,//3+2+1
					ACTION_LIMIT = 3,
					BLINK_LIMIT = 12
					};

public:
	//moving average filter of the current eye position
	//array[0] is the current, averaged position
	//array[1] to array[MAF_LENGTH] stores the previous (MAF_LENGTH-1) values
	int* currentGazePositionX;
	int* currentGazePositionY;
	int* currentEyePositionX;
	int* currentEyePositionY;
	int* currentEyeSizeX;
	int* currentEyeSizeY;
	int* currentPupilRadius;

	int currentLeftEyeAction;
	int currentLeftEyeActionCount;
	int currentRightEyeAction;
	int currentRightEyeActionCount;
	int currentBrowAction;
	int blinkFrameCount;
	//when a new eye is given, this updates the stored values in the MAF
	//and computes the new average position
	void updateCurrentEyePosition(Eye eye);
	int updateState(Eye newEyeLeft, Eye newEyeRight);
	int calculateCurrentEyePosition();
	//the constructor
	Recognizer();
	~Recognizer();
	

};

#endif