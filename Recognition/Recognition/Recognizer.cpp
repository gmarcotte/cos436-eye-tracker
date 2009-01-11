

#include "Recognizer.h"

Recognizer::Recognizer()
{
	blinkFrameCount = 0;

	currentLeftEyeAction = Recognizer::CENTER;
	currentRightEyeAction = Recognizer::CENTER;
	currentBrowAction = Recognizer::RAISED;

	currentGazePositionX = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentGazePositionY = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentEyePositionX = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentEyePositionY = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentEyeSizeX = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentEyeSizeY = (int*) malloc(sizeof(int) * MAF_LENGTH);
	currentPupilRadius = (int*) malloc(sizeof(int) * MAF_LENGTH);
	//check memory
	if(currentEyeSizeY == NULL){printf("out of memory\n");return;}

	for(int i = 0; i < MAF_LENGTH-1; i++)
	{
		currentGazePositionX[i] = 0;
		currentGazePositionY[i] = 0;
		currentEyePositionX[i] = 0;
		currentEyePositionY[i] = 0;
		currentEyeSizeX[i] = 0;
		currentEyeSizeY[i] = 0;
		currentPupilRadius[i] = 0;
	}
}

//when a new eye is given, this updates the stored values in the MAF
//and computes the new average position
void Recognizer::updateCurrentEyePosition(Eye eye)
{
	//put in error check here for freak out case
	//basically compute the distance from the last one
	//and if it is huge (more than a few Pupil radii), ignore it

	int totalX = 0;
	int totalY = 0;
	int totalEPX = 0;
	int totalEPY = 0;
	int totalESX = 0;
	int totalESY = 0;
	int totalP = 0;

	for(int i = (MAF_LENGTH - 2); i > 1 ; i--)
	{
		currentGazePositionX[i] = currentGazePositionX[i-1]; 
		currentGazePositionY[i] = currentGazePositionY[i-1];
		currentEyePositionX[i] = currentEyePositionX[i-1];
		currentEyePositionY[i] = currentEyePositionY[i-1];
		currentEyeSizeX[i] = currentEyeSizeX[i-1];
		currentEyeSizeY[i] = currentEyeSizeY[i-1];
		currentPupilRadius[i] = currentPupilRadius[i-1];

		totalX += currentGazePositionX[i]*(MAF_LENGTH - i -1);
		totalY += currentGazePositionY[i]*(MAF_LENGTH - i -1);
		totalEPX += currentEyePositionX[i]*(MAF_LENGTH-i-1);
		totalEPY += currentEyePositionY[i]*(MAF_LENGTH-i-1);
		totalESX += currentEyeSizeX[i]*(MAF_LENGTH-i-1);
		totalESY += currentEyeSizeY[i]*(MAF_LENGTH-i-1);
		totalP += currentPupilRadius[i]*(MAF_LENGTH-i-1); 
	}
	currentGazePositionX[1] = eye.pupilPositionX;
	currentGazePositionY[1] = eye.pupilPositionY;
	currentEyePositionX[1] = eye.eyePositionX;
	currentEyePositionY[1] = eye.eyePositionY;
	currentEyeSizeX[1] = eye.eyeSizeX;
	currentEyeSizeY[1] = eye.eyeSizeY;
	currentPupilRadius[1] = eye.pupilRadius;

	totalX += currentGazePositionX[1]*(MAF_LENGTH-1);
	totalY += currentGazePositionY[1]*(MAF_LENGTH-1);
	totalEPX += currentEyePositionX[1]*(MAF_LENGTH-1);
	totalEPY += currentEyePositionY[1]*(MAF_LENGTH-1);
	totalESX += currentEyeSizeX[1]*(MAF_LENGTH-1);
	totalESY += currentEyeSizeY[1]*(MAF_LENGTH-1);
	totalP += currentPupilRadius[1]*(MAF_LENGTH-1);
	
	currentGazePositionX[0] = (int) totalX/(MAF_DIVISOR);
	currentGazePositionY[0] = (int) totalY/(MAF_DIVISOR);
	currentEyePositionX[0] = (int) totalEPX/(MAF_DIVISOR);
	currentEyePositionY[0] = (int) totalEPY/(MAF_DIVISOR);
	currentEyeSizeX[0] = (int) totalESX/(MAF_DIVISOR);
	currentEyeSizeY[0] = (int) totalESY/(MAF_DIVISOR);
	currentPupilRadius[0] = (int) totalP/(MAF_DIVISOR);
}
int calculateCurrentEyePosition()
{
	//get the center of the eye image
	int eyeHalfX = this.currentEyePositionX[0] + currentEyeSizeX[0]/2;
	int eyeHalfY = currentEyePositionY[0] + currentEyeSizeY[0]/2;
	//get the other side of the eye image
	int eyeFarX = currentEyePositionX[0] + currentEyeSizeX[0];
	int eyeFarY = currentEyePositionY[0] + currentEyeSizeY[0];
	//get the bounds of the pupil
	int pupilTop = currentGazePositionY[0] + currentPupilRadius[0];
	int pupilBottom = currentGazePositionY[0] - currentPupilRadius[0];
	int pupilLeft = currentGazePositionX[0] - currentPupilRadius[0];
	int pupilRight = currentGazePositionX[0] + currentPupilRadius[0];

	if(pupilRight>eyeFarX && currentGazePositionX[0]>eyeHalfX)
	{
		return Eye::LEFT;//the pupil is far right in the image,
		            //therefore the person is looking left
	}
	else if(pupilLeft<currentEyePositionX[0] && currentGazePositionX[0]<eyeHalfX)
	{
		return Eye::RIGHT;//the pupil is far left in the image,
		            //therefore the person is looking right
	}
	else if(pupilBottom>eyeHalfY && pupilTop>currentEyePositionY[0])
	{
		return Eye::UP;
	}
	else if(pupilTop<eyeHalfY && pupilBottom<currentEyePositionY[0])
	{
		return Eye::DOWN;
	}
	else return Eye::CENTER;
}

int Recognizer::updateState(Eye newEyeLeft, Eye newEyeRight)
{
	int newState;
	if(!newEyeLeft.noPupilDetected)
	{
		blinkFrameCount = 0;

		//first store the old averaged position;
		int oldGazePositionX = currentGazePositionX[0];
		int oldGazePositionY = currentGazePositionY[0];
		//then update the gaze position
		updateCurrentEyePosition(newEyeLeft);
		
		newState = calculateCurrentEyePosition();
		if(newState == currentLeftEyeAction)
		{
			currentLeftEyeActionCount++;
			if(currentLeftEyeActionCount>ACTION_LIMIT)
			{//send the appropriate key
				switch(currentLeftEyeActionCount)
				{
				case Eye::LEFT:
					System::Windows::Forms::SendKeys::SendWait("a");
					break;
				case Eye::RIGHT:
					System::Windows::Forms::SendKeys::SendWait("d");
					break;
				case Eye::UP:
					System::Windows::Forms::SendKeys::SendWait("w");
					break;
				case Eye::DOWN:
					System::Windows::Forms::SendKeys::SendWait("s");
					break;
				}
			}
			currentLeftEyeActionCount = 0;
		}
		else
		{
			currentLeftEyeActionCount = 0;
			currentLeftEyeAction = newState;
		}
	}
	else
	{
		//HANDLE BLINK CASES!
		blinkFrameCount++;
		if(blinkFrameCount>=BLINK_LIMIT)
		{
			currentLeftEyeAction = LEFT_CLOSED;
			System::Windows::Forms::SendKeys::SendWait("q");
			blinkFrameCount=0;
		}
	}
	return CENTER;
}

//Deconstructor
Recognizer::~Recognizer()
{
	free(currentGazePositionX);
	free(currentGazePositionY);
}


//*/