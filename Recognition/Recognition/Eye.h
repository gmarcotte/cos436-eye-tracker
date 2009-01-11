#ifndef EYE_CLASS
#define EYE_CLASS


class Eye{
public:
	enum eyePositions {CENTER, LEFT, RIGHT, UP, DOWN};

	bool noPupilDetected;
	int pupilPositionX;
	int pupilPositionY;
	int pupilRadius;
	int eyeSizeX;
	int eyeSizeY;
	int eyePositionX;
	int eyePositionY;
	int browPositionY;
	int getRelativeEyePositionX();
	int getRelativeEyePositionY();
	//is the eye looking up, down, left, right or center?
	int returnEyePosition();
	Eye();
	Eye(bool noPupDet, int pupPosX, int pupPosY,
		int pupRad, int eyeSzX, int eyeSzY, int eyePosX,
		int eyePosY, int browPosY);
};

#endif