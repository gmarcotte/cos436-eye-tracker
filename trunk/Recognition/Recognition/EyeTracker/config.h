/* Configuration settings for the eye tracking application */


// Tracking configurations
enum {
	NUM_INPUTS				= 2,
	MAX_STRING				= 255,
	FLIP_INPUT				= 1,
	MIN_PUPIL_RADIUS  = 5,
	MIN_EB_PERIM			= 100,
	CANNY_LOW					= 200,
	CANNY_HIGH				= 250,
};


// Available actions
enum {
	EYE_LEFT,
	EYE_RIGHT,
	EYE_UP,
	EYE_DOWN,
	LEFT_WINK,
	RIGHT_WINK,
	BLINK,
	EYEBROW_RAISE,
	NUM_ACTIONS
}



