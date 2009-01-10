class ConfigTrackbar {
private:
	char* trackbar_name;
	char* window_name;
	double min_value;
	double max_value;
	int num_steps;
	int position;

public:

	// Constructor
	ConfigTrackbar(
		char* trackbar_name, char* window_name, 
		double min_value, double max_value, double start_value,
		int num_steps);

	// Accessors
	double getValue();
	int setValue(double val);
};