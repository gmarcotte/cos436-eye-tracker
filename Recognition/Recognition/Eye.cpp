
#include "Eye.h"

Eye::Eye()
{
	  noPupilDetected = false;
	  pupilPositionX=0;
	  pupilPositionY=0;
	  pupilRadius=0;
	  eyeSizeX=0;
	  eyeSizeY=0;
	  eyePositionX=0;
	  eyePositionY=0;
	  browPositionY=0;
}
int Eye::getRelativeEyePositionX()
{
	return pupilPositionX - eyePositionX;
}
int Eye::getRelativeEyePositionY()
{
	return pupilPositionY - eyePositionY;
}
int Eye::returnEyePosition()
{
	//get the center of the eye image
	int eyeHalfX = eyePositionX + eyeSizeX/2;
	int eyeHalfY = eyePositionY + eyeSizeY/2;
	//get the other side of the eye image
	int eyeFarX = eyePositionX + eyeSizeX;
	int eyeFarY = eyePositionY + eyeSizeY;
	//get the bounds of the pupil
	int pupilTop = pupilPositionY + pupilRadius;
	int pupilBottom = pupilPositionY - pupilRadius;
	int pupilLeft = pupilPositionX - pupilRadius;
	int pupilRight = pupilPositionX + pupilRadius;

	if(pupilRight>eyeFarX && pupilPositionX>eyeHalfX)
	{
		return LEFT;//the pupil is far right in the image,
		            //therefore the person is looking left
	}
	else if(pupilLeft<eyePositionX && pupilPositionX<eyeHalfX)
	{
		return RIGHT;//the pupil is far left in the image,
		            //therefore the person is looking right
	}
	else if(pupilBottom>eyeHalfY && pupilTop>eyePositionY)
	{
		return UP;
	}
	else if(pupilTop<eyeHalfY && pupilBottom<eyePositionY)
	{
		return DOWN;
	}
	else return CENTER;
}