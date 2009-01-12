
#include <stdio.h>
#include "Recognizer.h"

Recognizer::Recognizer()
{
	blinkFrameCountLeft = 0;
	blinkFrameCountRight = 0;

	currentLeftEyeAction = Recognizer::CENTER;
	currentRightEyeAction = Recognizer::CENTER;
	currentBrowAction = Recognizer::RAISED;

		//CALIBRATION VARIABLES
	calibrationPupilTopL=0;
	calibrationPupilBottomL=0;
	calibrationPupilLeftL=0;
	calibrationPupilRightL=0;
	calibrationBrowL=0;
	calibrationCountPupilTopL=0;
	calibrationCountPupilBottomL=0;
	calibrationCountPupilLeftL=0;
	calibrationCountPupilRightL=0;
	calibrationCountBrowL=0;
		//CALIBRATION VARIABLES
	calibrationPupilTopR=0;
	calibrationPupilBottomR=0;
	calibrationPupilLeftR=0;
	calibrationPupilRightR=0;
	calibrationBrowR=0;
	calibrationCountPupilTopR=0;
	calibrationCountPupilBottomR=0;
	calibrationCountPupilLeftR=0;
	calibrationCountPupilRightR=0;
	calibrationCountBrowR=0;

	//Left Eye
	currentGazePositionLX = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentGazePositionLY = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentEyePositionLX = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentEyePositionLY = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentEyeSizeLX = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentEyeSizeLY = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentPupilRadiusL = (int*) malloc(sizeof(int) * MAF_LENGTH);
	//Right Eye
	currentGazePositionRX = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentGazePositionRY = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentEyePositionRX = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentEyePositionRY = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentEyeSizeRX = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentEyeSizeRY = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentPupilRadiusR = (int*) malloc(sizeof(int) * MAF_LENGTH);
	//brows
	browPositionL = (int*) malloc(sizeof(int) * MAF_LENGTH);
	browPositionR = (int*) malloc(sizeof(int) * MAF_LENGTH);

	//check memory
	if(browPositionR == NULL){printf("out of memory\n");return;}

	for(int i = 0; i < MAF_LENGTH-1; i++)
	{
		currentGazePositionLX[i] = 0;
		currentGazePositionLY[i] = 0;
		currentEyePositionLX[i] = 0;
		currentEyePositionLY[i] = 0;
		currentEyeSizeLX[i] = 0;
		currentEyeSizeLY[i] = 0;
		currentPupilRadiusL[i] = 0;

		currentGazePositionRX[i] = 0;
		currentGazePositionRY[i] = 0;
		currentEyePositionRX[i] = 0;
		currentEyePositionRY[i] = 0;
		currentEyeSizeRX[i] = 0;
		currentEyeSizeRY[i] = 0;
		currentPupilRadiusR[i] = 0;

		browPositionL[i] = 0;
		browPositionR[i] = 0;
	}
}

//when a new eye is given, this updates the stored values in the MAF
//and computes the new average position
void Recognizer::updateCurrentLeftEyePosition(Eye leftEye)
{
	//put in error check here for freak out case
	//basically compute the distance from the last one
	//and if it is huge (more than a few Pupil radii), ignore it

	int totalLX = 0;
	int totalLY = 0;
	int totalLEPX = 0;
	int totalLEPY = 0;
	int totalLESX = 0;
	int totalLESY = 0;
	int totalLP = 0;

	for(int i = (MAF_LENGTH - 1); i > 1 ; i--)
	{
		//first left
		currentGazePositionLX[i] = currentGazePositionLX[i-1]; 
		currentGazePositionLY[i] = currentGazePositionLY[i-1];
		currentEyePositionLX[i] = currentEyePositionLX[i-1];
		currentEyePositionLY[i] = currentEyePositionLY[i-1];
		currentEyeSizeLX[i] = currentEyeSizeLX[i-1];
		currentEyeSizeLY[i] = currentEyeSizeLY[i-1];
		currentPupilRadiusL[i] = currentPupilRadiusL[i-1];

		totalLX += currentGazePositionLX[i]*(MAF_LENGTH - i -1);
		totalLY += currentGazePositionLY[i]*(MAF_LENGTH - i -1);
		totalLEPX += currentEyePositionLX[i]*(MAF_LENGTH-i-1);
		totalLEPY += currentEyePositionLY[i]*(MAF_LENGTH-i-1);
		totalLESX += currentEyeSizeLX[i]*(MAF_LENGTH-i-1);
		totalLESY += currentEyeSizeLY[i]*(MAF_LENGTH-i-1);
		totalLP += currentPupilRadiusL[i]*(MAF_LENGTH-i-1); 
	}
	//handle edge cases
	//first left
	currentGazePositionLX[1] = leftEye.pupilPositionX;
	currentGazePositionLY[1] = leftEye.pupilPositionY;
	currentEyePositionLX[1] = leftEye.eyePositionX;
	currentEyePositionLY[1] = leftEye.eyePositionY;
	currentEyeSizeLX[1] = leftEye.eyeSizeX;
	currentEyeSizeLY[1] = leftEye.eyeSizeY;
	currentPupilRadiusL[1] = leftEye.pupilRadius;

	totalLX += currentGazePositionLX[1]*(MAF_LENGTH-1);
	totalLY += currentGazePositionLY[1]*(MAF_LENGTH-1);
	totalLEPX += currentEyePositionLX[1]*(MAF_LENGTH-1);
	totalLEPY += currentEyePositionLY[1]*(MAF_LENGTH-1);
	totalLESX += currentEyeSizeLX[1]*(MAF_LENGTH-1);
	totalLESY += currentEyeSizeLY[1]*(MAF_LENGTH-1);
	totalLP += currentPupilRadiusL[1]*(MAF_LENGTH-1);
	
	currentGazePositionLX[0] = (int) totalLX/(MAF_DIVISOR);
	currentGazePositionLY[0] = (int) totalLY/(MAF_DIVISOR);
	currentEyePositionLX[0] = (int) totalLEPX/(MAF_DIVISOR);
	currentEyePositionLY[0] = (int) totalLEPY/(MAF_DIVISOR);
	currentEyeSizeLX[0] = (int) totalLESX/(MAF_DIVISOR);
	currentEyeSizeLY[0] = (int) totalLESY/(MAF_DIVISOR);
	currentPupilRadiusL[0] = (int) totalLP/(MAF_DIVISOR);
	
	
}
void Recognizer::updateCurrentRightEyePosition(Eye rightEye)
{
	//put in error check here for freak out case
	//basically compute the distance from the last one
	//and if it is huge (more than a few Pupil radii), ignore it

	int totalRX = 0;
	int totalRY = 0;
	int totalREPX = 0;
	int totalREPY = 0;
	int totalRESX = 0;
	int totalRESY = 0;
	int totalRP = 0;

	for(int i = (MAF_LENGTH - 1); i > 1 ; i--)
	{

		//now right
		currentGazePositionRX[i] = currentGazePositionRX[i-1]; 
		currentGazePositionRY[i] = currentGazePositionRY[i-1];
		currentEyePositionRX[i] = currentEyePositionRX[i-1];
		currentEyePositionRY[i] = currentEyePositionRY[i-1];
		currentEyeSizeRX[i] = currentEyeSizeRX[i-1];
		currentEyeSizeRY[i] = currentEyeSizeRY[i-1];
		currentPupilRadiusR[i] = currentPupilRadiusR[i-1];

		totalRX += currentGazePositionRX[i]*(MAF_LENGTH - i -1);
		totalRY += currentGazePositionRY[i]*(MAF_LENGTH - i -1);
		totalREPX += currentEyePositionRX[i]*(MAF_LENGTH-i-1);
		totalREPY += currentEyePositionRY[i]*(MAF_LENGTH-i-1);
		totalRESX += currentEyeSizeRX[i]*(MAF_LENGTH-i-1);
		totalRESY += currentEyeSizeRY[i]*(MAF_LENGTH-i-1);
		totalRP += currentPupilRadiusR[i]*(MAF_LENGTH-i-1); 

	}
	//handle edge cases
	
	//now right
	currentGazePositionRX[1] = rightEye.pupilPositionX;
	currentGazePositionRY[1] = rightEye.pupilPositionY;
	currentEyePositionRX[1] = rightEye.eyePositionX;
	currentEyePositionRY[1] = rightEye.eyePositionY;
	currentEyeSizeRX[1] = rightEye.eyeSizeX;
	currentEyeSizeRY[1] = rightEye.eyeSizeY;
	currentPupilRadiusR[1] = rightEye.pupilRadius;

	totalRX += currentGazePositionRX[1]*(MAF_LENGTH-1);
	totalRY += currentGazePositionRY[1]*(MAF_LENGTH-1);
	totalREPX += currentEyePositionRX[1]*(MAF_LENGTH-1);
	totalREPY += currentEyePositionRY[1]*(MAF_LENGTH-1);
	totalRESX += currentEyeSizeRX[1]*(MAF_LENGTH-1);
	totalRESY += currentEyeSizeRY[1]*(MAF_LENGTH-1);
	totalRP += currentPupilRadiusR[1]*(MAF_LENGTH-1);
	
	currentGazePositionRX[0] = (int) totalRX/(MAF_DIVISOR);
	currentGazePositionRY[0] = (int) totalRY/(MAF_DIVISOR);
	currentEyePositionRX[0] = (int) totalREPX/(MAF_DIVISOR);
	currentEyePositionRY[0] = (int) totalREPY/(MAF_DIVISOR);
	currentEyeSizeRX[0] = (int) totalRESX/(MAF_DIVISOR);
	currentEyeSizeRY[0] = (int) totalRESY/(MAF_DIVISOR);
	currentPupilRadiusR[0] = (int) totalRP/(MAF_DIVISOR);
}

int Recognizer::calculateCurrentLeftEyePosition()
{
	//
	//get the center of the eye image
	int eyeHalfX = currentEyePositionLX[0] + (int) currentEyeSizeLX[0]/2;
	int eyeHalfY = currentEyePositionLY[0] + (int) currentEyeSizeLY[0]/2;
	//get the other side of the eye image
	int eyeFarX = currentEyePositionLX[0] + currentEyeSizeLX[0];
	int eyeFarY = currentEyePositionLY[0] + currentEyeSizeLY[0];
		//get the bounds of the pupil
	int pupilTop = currentGazePositionLY[0] + (int) currentPupilRadiusL[0]/2;
	int pupilBottom = currentGazePositionLY[0] - (int) currentPupilRadiusL[0]/2;
	int pupilLeft = currentGazePositionLX[0] + currentPupilRadiusL[0];
	int pupilRight = currentGazePositionLX[0] - currentPupilRadiusL[0];

	if(pupilRight<=calibrationPupilRightL)
	{
		return Eye::LEFT;//the pupil is far right in the image,
		            //therefore the person is looking left
	}
	else if(pupilLeft>=calibrationPupilLeftL)
	{
		return Eye::RIGHT;//the pupil is far left in the image,
		            //therefore the person is looking right
	}
	else if(pupilBottom >= calibrationPupilBottomL)
	{
		return Eye::UP;
	}
	else if(pupilTop <= calibrationPupilTopL)
	{
		return Eye::DOWN;
	}
	else return Eye::CENTER;
}
int Recognizer::calculateCurrentRightEyePosition()
{
	//
	//get the center of the eye image
	int eyeHalfX = currentEyePositionRX[0] + (int) currentEyeSizeRX[0]/2;
	int eyeHalfY = currentEyePositionRY[0] + (int) currentEyeSizeRY[0]/2;
	//get the other side of the eye image
	int eyeFarX = currentEyePositionRX[0] + currentEyeSizeRX[0];
	int eyeFarY = currentEyePositionRY[0] + currentEyeSizeRY[0];
	//get the bounds of the pupil
	int pupilTop = currentGazePositionRY[0] + (int) currentPupilRadiusR[0]/2;
	int pupilBottom = currentGazePositionRY[0] - (int) currentPupilRadiusR[0]/2;
	int pupilLeft = currentGazePositionRX[0] + currentPupilRadiusR[0];
	int pupilRight = currentGazePositionRX[0] - currentPupilRadiusR[0];

	if(pupilRight <= calibrationPupilRightR)
	{
		return Eye::LEFT;//the pupil is far right in the image,
		            //therefore the person is looking left
	}
	else if(pupilLeft >= calibrationPupilLeftR)
	{
		return Eye::RIGHT;//the pupil is far left in the image,
		            //therefore the person is looking right
	}
	else if(pupilBottom >= calibrationPupilBottomR)
	{
		return Eye::UP;
	}
	else if(pupilTop <= calibrationPupilTopR)
	{
		return Eye::DOWN;
	}
	else return Eye::CENTER;
}

int Recognizer::updateState(Eye newEyeLeft, Eye newEyeRight)
{
	int newStateL;
	int newStateR;
	if(!newEyeLeft.noPupilDetected && !newEyeRight.noPupilDetected)
	{//neither eye is blinking
		blinkFrameCountLeft = 0;
		blinkFrameCountRight=0;
		//first store the old averaged position;
		int oldGazePositionLX = currentGazePositionLX[0];
		int oldGazePositionLY = currentGazePositionLY[0];
		int oldGazePositionRX = currentGazePositionRX[0];
		int oldGazePositionRY = currentGazePositionRY[0];
		//then update the gaze position
		updateCurrentLeftEyePosition(newEyeLeft);
		updateCurrentRightEyePosition(newEyeLeft);
		
		newStateL = calculateCurrentLeftEyePosition();
		newStateR = calculateCurrentRightEyePosition();
		if(newStateL == currentLeftEyeAction)
		{
			currentLeftEyeActionCount++;
			if(currentLeftEyeActionCount>ACTION_LIMIT)
			{//send the appropriate key
				printf("neither is blinking, left is sending a %d,\n", currentLeftEyeAction);
				//output(currentLeftEyeAction);
			//	currentLeftEyeActionCount = 0;
			}
		}
		else
		{
			currentLeftEyeActionCount = 0;
			currentLeftEyeAction = newStateL;
		}

		if(newStateR == currentRightEyeAction)
		{
			currentRightEyeActionCount++;
			if(currentRightEyeActionCount>ACTION_LIMIT)
			{//send the appropriate key
				printf("neither is blinking, right is sending a %d,\n", currentRightEyeAction);
				//output(currentRightEyeAction);
				//currentRightEyeActionCount = 0;
			}
		}
		else
		{
			currentRightEyeActionCount = 0;
			currentRightEyeAction = newStateR;
		}

		if(currentRightEyeAction == currentLeftEyeAction)
		{
			if(currentRightEyeActionCount>ACTION_LIMIT && currentLeftEyeActionCount>ACTION_LIMIT)
			{
				printf("Officially sending, the eye just given", newEyeLeft.pupilPositionX, newEyeLeft.pupilRadius);
				output(currentRightEyeAction);
			}
		}
	}
	else if(newEyeLeft.noPupilDetected && !newEyeRight.noPupilDetected)
	{//left eye is blinking, not right
		//Get the Position of the Right Eye!
		newStateR = calculateCurrentRightEyePosition();
		if(newStateR == currentRightEyeAction)
		{
			currentRightEyeActionCount++;
			if(currentRightEyeActionCount>ACTION_LIMIT)
			{//send the appropriate key
				printf("left is blinking, right is sending a %d,\n", currentRightEyeAction);
				output(currentRightEyeAction);
				currentRightEyeActionCount = 0;
			}
		}
		else
		{
			currentRightEyeActionCount = 0;
			currentRightEyeAction = newStateR;
		}

		blinkFrameCountRight=0;
		//HANDLE BLINK CASES!
		blinkFrameCountLeft++;
		if(blinkFrameCountLeft>=BLINK_LIMIT)
		{
			currentLeftEyeAction = LEFT_CLOSED;
			printf("Printing e\n");
			System::Windows::Forms::SendKeys::SendWait("e");
			blinkFrameCountLeft=0;
		}
	}
	else if(newEyeRight.noPupilDetected && !newEyeLeft.noPupilDetected)
	{//right eye is blinking, not left
		//get the position of the left eye!
		newStateL = calculateCurrentLeftEyePosition();
		if(newStateL == currentLeftEyeAction)
		{
			currentLeftEyeActionCount++;
			if(currentLeftEyeActionCount>ACTION_LIMIT)
			{//send the appropriate key
				printf("right is blinking, left is sending a %d,\n", currentLeftEyeAction);
				output(currentLeftEyeAction);
				currentLeftEyeActionCount = 0;
			}
		}
		else
		{
			currentLeftEyeActionCount = 0;
			currentLeftEyeAction = newStateL;
		}
		//HANDLE BLINK CASES!
		blinkFrameCountRight++;
		if(blinkFrameCountRight>=BLINK_LIMIT)
		{
			currentRightEyeAction = RIGHT_CLOSED;
			printf("Sending q\n");
			System::Windows::Forms::SendKeys::SendWait("q");
			blinkFrameCountRight=0;
		}
	}
	else if(newEyeRight.noPupilDetected && newEyeLeft.noPupilDetected)
	{//both eyes are blinking!
		//HANDLE BLINK CASES!
		blinkFrameCountRight++;
		blinkFrameCountLeft++;
		if(blinkFrameCountRight>=BLINK_LIMIT && blinkFrameCountLeft>=BLINK_LIMIT)
		{
			currentRightEyeAction = RIGHT_CLOSED;
			currentLeftEyeAction = LEFT_CLOSED;
			printf("Sending r\n");
			System::Windows::Forms::SendKeys::SendWait("r");
			blinkFrameCountRight=0;
			blinkFrameCountLeft=0;
		}
		else if(blinkFrameCountRight>=BLINK_LIMIT)
		{
			currentRightEyeAction = RIGHT_CLOSED;
			printf("Sending q\n");
			System::Windows::Forms::SendKeys::SendWait("q");
			blinkFrameCountRight=0;
		}
		else if(blinkFrameCountLeft>=BLINK_LIMIT)
		{
			currentLeftEyeAction = LEFT_CLOSED;
			printf("Sending e\n");
			System::Windows::Forms::SendKeys::SendWait("e");
			blinkFrameCountLeft=0;
		}
	}
	
	//now do some brows
	bool rightIsRaised = false;
	bool leftIsRaised = false;
	for(int i = (MAF_LENGTH - 1); i > 0 ; i--)
	{
		browPositionL[i] = browPositionL[i-1]; 
		browPositionR[i] = browPositionR[i-1];
	}
	browPositionL[0] = newEyeLeft.browPositionY;
	browPositionR[0] = newEyeRight.browPositionY;
	if( (browPositionL[0] + (int) .5*newEyeLeft.browHeight) < calibrationBrowL)
	{
		leftIsRaised = true;
	}
	if( (browPositionR[0] + (int) .5*newEyeRight.browHeight) < calibrationBrowL)
	{
		rightIsRaised = true;
	}
	if(leftIsRaised && rightIsRaised)
	{
		System::Windows::Forms::SendKeys::SendWait("f");
	}
	

	return currentLeftEyeAction;
}

void Recognizer::printInfo()
{
	printf("LpupilLeft:%d, pupilRight:%d\n", calibrationPupilLeftL, calibrationPupilRightL); 
	printf("RpupilLeft:%d, pupilRight:%d\n", calibrationPupilLeftR, calibrationPupilRightR); 
	printf("currentPosRad:%d,%d\n", currentGazePositionLX[0], currentPupilRadiusL[0]);
	printf("lastPosRad:%d,%d\n", currentGazePositionLX[1], currentPupilRadiusL[1]);
}
void Recognizer::output(int currentEyeAction)
{
	switch(currentEyeAction)
	{
	case Eye::LEFT:
	{
		printf("Sending d\n");
		printInfo();
		System::Windows::Forms::SendKeys::SendWait("d");
		break;
	}
	case Eye::RIGHT:
	{
		printf("Sending a\n");
		printInfo();
		System::Windows::Forms::SendKeys::SendWait("a");
		break;
	}
	case Eye::UP:
	{
		printf("Sending w\n");
		System::Windows::Forms::SendKeys::SendWait("w");
		break;
	}
	case Eye::DOWN:
	{
		printf("Sending s\n");
		System::Windows::Forms::SendKeys::SendWait("s");
		break;
	}
	}
}

int newAvg(int oldAvg, int oldCount, int newAdd)
{
	return (oldCount*oldAvg + newAdd ) / ( oldCount+1);
}
void Recognizer::updateLeftCalibration(Eye sourceEye, int calibrationType)
{
	switch(calibrationType)
	{
		case EYE_LEFT:
			calibrationPupilLeftL = newAvg(calibrationPupilLeftL, calibrationCountPupilLeftL,  sourceEye.eyePositionX);
			calibrationCountPupilLeftL++;
			break;
		case EYE_RIGHT:
			calibrationPupilRightL = newAvg(calibrationPupilRightL, calibrationCountPupilRightL,  sourceEye.eyePositionX);
			calibrationCountPupilRightL++;
			break;
		case EYE_UP:
			calibrationPupilTopL = newAvg(calibrationPupilTopL, calibrationCountPupilTopL,  sourceEye.eyePositionY);
			calibrationCountPupilTopL++;
			break;
		case EYE_DOWN:
			calibrationPupilBottomL = newAvg(calibrationPupilBottomL, calibrationCountPupilBottomL,  sourceEye.eyePositionY);
			calibrationCountPupilBottomL++;
			break;
		case LEFT_WINK:
			break;
		case RIGHT_WINK:
			break;
		case BLINK:
			break;
		case EYEBROW_RAISE:
			calibrationBrowL = newAvg(calibrationBrowL, calibrationCountBrowL,  sourceEye.browPositionY);
			calibrationCountBrowL++;
			break;
	}
}
void Recognizer::updateRightCalibration(Eye sourceEye, int calibrationType)
{
	switch(calibrationType)
	{
		case EYE_LEFT:
			calibrationPupilLeftR = newAvg(calibrationPupilLeftR, calibrationCountPupilLeftR,  sourceEye.eyePositionX);
			calibrationCountPupilLeftR++;
			break;
		case EYE_RIGHT:
			calibrationPupilRightR = newAvg(calibrationPupilRightR, calibrationCountPupilRightR,  sourceEye.eyePositionX);
			calibrationCountPupilRightR++;
			break;
		case EYE_UP:
			calibrationPupilTopR = newAvg(calibrationPupilTopR, calibrationCountPupilTopR,  sourceEye.eyePositionY);
			calibrationCountPupilTopR++;
			break;
		case EYE_DOWN:
			calibrationPupilBottomR = newAvg(calibrationPupilBottomR, calibrationCountPupilBottomR,  sourceEye.eyePositionY);
			calibrationCountPupilBottomR++;
			break;
		case LEFT_WINK:
			break;
		case RIGHT_WINK:
			break;
		case BLINK:
			break;
		case EYEBROW_RAISE:
			calibrationBrowR = newAvg(calibrationBrowR, calibrationCountBrowR,  sourceEye.browPositionY);
			calibrationCountBrowR++;
			break;
	}
}

//Deconstructor
Recognizer::~Recognizer()
{
	//Get rid of allocated MAF stuff
	//left ones
	free(currentGazePositionLX);
	free(currentGazePositionLY);
	free(currentEyePositionLX);
	free(currentEyePositionLY);
	free(currentEyeSizeLX);
	free(currentEyeSizeLY);
	free(currentPupilRadiusL);
	//right ones
	free(currentGazePositionRX);
	free(currentGazePositionRY);
	free(currentEyePositionRX);
	free(currentEyePositionRY);
	free(currentEyeSizeRX);
	free(currentEyeSizeRY);
	free(currentPupilRadiusR);
}


//*/