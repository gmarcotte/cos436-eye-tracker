/* Configuration settings for the eye tracking application */
#ifndef CONFIG_H
#define CONFIG_H

// Tracking configurations
enum {
	NUM_INPUTS				= 2,
	MAX_STRING				= 255,
	FLIP_INPUT				= 1,
	IGNORE_PUPIL_RADIUS = 8,
	MIN_PUPIL_RADIUS  = 3,
	MIN_EB_PERIM			= 100,
	CANNY_LOW					= 200,
	CANNY_HIGH				= 250,
	CALIBRATION_FRAMES = 10,
	NUM_TO_CALIBRATE  = 5,
};


// Available actions
enum {
	EYE_LEFT = 0,
	EYE_RIGHT,
	EYE_UP,
	EYE_DOWN,
	EYEBROW_RAISE,
	LEFT_WINK,
	RIGHT_WINK,
	BLINK,
	NUM_ACTIONS
};



#endif