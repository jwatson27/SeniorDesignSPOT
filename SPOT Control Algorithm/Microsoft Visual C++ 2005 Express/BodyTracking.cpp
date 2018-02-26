#include "stdafx.h"
#include "Form1.h"


int main(array<System::String ^> ^args) {
	HIDPnPDemo::Application::EnableVisualStyles();
	HIDPnPDemo::Application::SetCompatibleTextRenderingDefault(false);
	
	// Create windows for displaying Kinect images
	cv::namedWindow("RGB Camera", cv::WINDOW_NORMAL);
	cv::namedWindow("Depth Sensor", cv::WINDOW_NORMAL);

	// Create the main GUI window and run it
	HIDPnPDemo::Application::Run(gcnew HIDPnPDemo::Form1());
	
	return 0;
}