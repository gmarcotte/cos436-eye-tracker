
#include <cv.h>
#include <highgui.h>
#include "GUI.h"

ConfigTrackbar::ConfigTrackbar(char* trackbar_name, char* window_name, double start_value,
															 double min_value, double max_value, int num_steps)
{
	this->trackbar_name = trackbar_name;
	this->window_name = window_name;
	this->min_value = min_value;
	this->max_value = max_value;
	this->num_steps = num_steps;
	this->position = 0;
	
	cvCreateTrackbar(trackbar_name, window_name, &(this->position), num_steps, NULL);
	
	if (!this->setValue(start_value))
		this->setValue(this->min_value);
}


double ConfigTrackbar::getValue()
{
	return (this->min_value + (this->max_value - this->min_value) * this->position / this->num_steps);
}

int ConfigTrackbar::setValue(double val)
{
	if (val > this->max_value || val < this->min_value)
		return 0;

	this->position = (int)((val - min_value) / num_steps);

	cvSetTrackbarPos(this->trackbar_name, this->window_name, this->position);
	return 1;
}