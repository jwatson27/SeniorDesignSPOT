#include "stdafx.h"
#include <Kinect.h>
#include <iostream>
#include "Form1.h"

void processBodies(const unsigned int &bodyCount, IBody **bodies);

template<class Interface>
static inline void safeRelease(Interface *&interfaceToRelease)
{
    if (interfaceToRelease != nullptr) {
        interfaceToRelease->Release();
        interfaceToRelease = nullptr;
    }
}

//const int MAX_DIFFBUFF_SIZE = 1000;
//double differences[MAX_DIFFBUFF_SIZE];
const int NUM_DIFFS = 4;
double lowestVal[NUM_DIFFS];
double highestVal[NUM_DIFFS];
int sampleCount = 0;
double avgDiff[NUM_DIFFS];
int activeBodyIndex = -1;

class Person {
	public:				
		Person(double fl, double fe, double ubl, double ube, 
			double lbl, double lbe, double nl, double ne) {			   
			faceLen = fl;
			upperBodyLen = ubl;
			lowerBodyLen = lbl;
			neckLen = nl;
			faceError = fe;
			upperBodyError = ube;
			lowerBodyError = lbe;
			neckError = ne;
		}

		double faceLen;		
		double upperBodyLen;
		double lowerBodyLen;
		double neckLen;

		double faceError;
		double upperBodyError;
		double lowerBodyError;
		double neckError;
};

Person jw = Person(16.3, 0.8, 25.3, 1.2, 36.0, 1.5, 8.1, 0.5);



int main(int argc, char *argv[])
{
	// Enabling Windows XP visual effects before any controls are created
	HIDPnPDemo::Application::EnableVisualStyles();
	HIDPnPDemo::Application::SetCompatibleTextRenderingDefault(false);

	// Create the main window and run it
	HIDPnPDemo::Application::Run(gcnew HIDPnPDemo::Form1());

    IKinectSensor *sensor = nullptr;	
	IMultiSourceFrameReader *multiSourceFrameReader = nullptr;

	for (int i = 0; i < NUM_DIFFS; i++) {
		lowestVal[i] = 10000;
		highestVal[i] = -10000;
		avgDiff[i] = -1;
	}


	
	

	//Get the default Kinect sensor
    HRESULT hr = GetDefaultKinectSensor(&sensor);

    //If the function succeeds, open the sensor
	if (SUCCEEDED(hr)) {
		hr = sensor->Open();
		
        if (SUCCEEDED(hr)) {
            //Get a body frame source from which we can get our body frame reader
			
			// TODO: can't get audio information
			hr = sensor->OpenMultiSourceFrameReader(
				(FrameSourceTypes_Color | FrameSourceTypes_Body | FrameSourceTypes_Depth
					| FrameSourceTypes_BodyIndex | FrameSourceTypes_Infrared),
				&multiSourceFrameReader);
        }
    }


    if (sensor == nullptr || FAILED(hr)) {
        std::cerr << "Cannot find any sensors.\n";
        return E_FAIL;
    }


	while (multiSourceFrameReader != nullptr) {
	
		IMultiSourceFrame *multiSourceFrame = nullptr;
		IBodyFrameReference *bodyFrameReference = nullptr;
		IBodyFrame *bodyFrame = nullptr;
		IColorFrameReference *colorFrameReference = nullptr;
		IColorFrame *colorFrame = nullptr;

		if (SUCCEEDED(hr)) {
			//TODO: Add checking for source types

			while (FAILED(hr = multiSourceFrameReader->AcquireLatestFrame(&multiSourceFrame)));

			// Get Body Frame
			hr = multiSourceFrame->get_BodyFrameReference(&bodyFrameReference);
			
			if (SUCCEEDED(hr)) {
				hr = bodyFrameReference->AcquireFrame(&bodyFrame);

				if (SUCCEEDED(hr)) {
					IBody *bodies[BODY_COUNT] = { 0 };
					hr = bodyFrame->GetAndRefreshBodyData(_countof(bodies), bodies);

					if (SUCCEEDED(hr)) {
						//std::cout << "Processing bodies!\n";
						processBodies(BODY_COUNT, bodies);
						//std::cout << "After processing bodies!\n";
						//After body processing is done, we're done with our bodies so release them.
						for (unsigned int bodyIndex = 0; bodyIndex < _countof(bodies); bodyIndex++) {
							safeRelease(bodies[bodyIndex]);
						}
						safeRelease(bodyFrame);
					}
				}
				else if (sensor) {
					BOOLEAN isSensorAvailable = false;
					hr = sensor->get_IsAvailable(&isSensorAvailable);
					if (SUCCEEDED(hr) && isSensorAvailable == false) {
						std::cerr << "No available sensor is found.\n";
					}
				}
				else {
					std::cerr << "Trouble reading the body frame.\n";
				}
			}


			

			// Get Color Frame
			//hr = multiSourceFrame->get_ColorFrameReference(&colorFrameReference);
			//
			//if (SUCCEEDED(hr)) {
			//	hr = colorFrameReference->AcquireFrame(&colorFrame);

			//	if (SUCCEEDED(hr)) {
			//		// Process Color Frame
			//		std::cout << "Got color frame";
			//		/*
			//		IBody *bodies[BODY_COUNT] = { 0 };
			//		hr = bodyFrame->GetAndRefreshBodyData(_countof(bodies), bodies);

			//		if (SUCCEEDED(hr)) {
			//			//std::cout << "Processing bodies!\n";
			//			processBodies(BODY_COUNT, bodies);
			//			//std::cout << "After processing bodies!\n";
			//			//After body processing is done, we're done with our bodies so release them.
			//			for (unsigned int bodyIndex = 0; bodyIndex < _countof(bodies); bodyIndex++) {
			//				safeRelease(bodies[bodyIndex]);
			//			}
			//			safeRelease(colorFrame);
			//		}
			//		*/
			//		safeRelease(colorFrame);
			//	}
			//	else if (sensor) {
			//		BOOLEAN isSensorAvailable = false;
			//		hr = sensor->get_IsAvailable(&isSensorAvailable);
			//		if (SUCCEEDED(hr) && isSensorAvailable == false) {
			//			std::cerr << "No available sensor is found.\n";
			//		}
			//	}
			//	else {
			//		std::cerr << "Trouble reading the color frame.\n";
			//	}
			//}			
						
			safeRelease(multiSourceFrame);
		}

		
		safeRelease(bodyFrameReference);
	}

	
	safeRelease(multiSourceFrameReader);

    return 0;
}

void processBodies(const unsigned int &bodyCount, IBody **bodies)
{
    for (unsigned int bodyIndex = 0; bodyIndex < bodyCount; bodyIndex++) {
        IBody *body = bodies[bodyIndex];

        //Get the tracking status for the body, if it's not tracked we'll skip it
        BOOLEAN isTracked = false;
        HRESULT hr = body->get_IsTracked(&isTracked);
        
		if (isTracked == false && bodyIndex == activeBodyIndex) {
			std::cout << "Reset average differences\n";
			for (int i = 0; i < NUM_DIFFS; i++) {
				avgDiff[i] = -1;
			}
		}

		if (FAILED(hr) || isTracked == false) {			
			continue;			
        }

		std::cout << "Body Index: " << bodyIndex << " \n";
		activeBodyIndex = bodyIndex;

        //If we're here the body is tracked so lets get the joint properties for this skeleton
        Joint joints[JointType_Count];
        hr = body->GetJoints(_countof(joints), joints);
        if (SUCCEEDED(hr)) {
			
            //Let's print the head's position
            /*
			const CameraSpacePoint &headPos = joints[JointType_Head].Position;
            const CameraSpacePoint &leftHandPos = joints[JointType_HandLeft].Position;

						
            //Let's check if the use has his hand up
            if (leftHandPos.Y >= headPos.Y) {
				std::cout << "leftHandPos: ";
				std::cout << "headPos.x: " << headPos.X << "\n";
				//std::cout << "LEFT HAND UP!!\n";
            }

            HandState leftHandState;
            hr = body->get_HandLeftState(&leftHandState);
            if (SUCCEEDED(hr)) {
                if (leftHandState == HandState_Closed) {
                    std::cout << "CLOSED HAND\n";
                }
                else if (leftHandState == HandState_Open) {
                    std::cout << "OPEN HAND\n";
                }
                else if (leftHandState == HandState_Lasso) {
                    std::cout << "PEW PEW HANDS\n";
                }
                else if (leftHandState == HandState_NotTracked) {
                    std::cout << "HAND IS NOT TRACKED\n";
                }
                else if (leftHandState == HandState_Unknown) {
                    std::cout << "HANDS STATE IS UNKNOWN\n";
                }
            }
			*/
			/*
			const CameraSpacePoint &spineBasePos		= joints[JointType_SpineBase].Position;		// 0
			const CameraSpacePoint &spineMidPos			= joints[JointType_SpineMid].Position;		// 1
			const CameraSpacePoint &neckPos				= joints[JointType_Neck].Position;			// 2
			const CameraSpacePoint &headPos				= joints[JointType_Head].Position;			// 3
			//const CameraSpacePoint &shoulderLeftPos		= joints[JointType_ShoulderLeft].Position;	// 4
			//const CameraSpacePoint &elbowLeftPos		= joints[JointType_ElbowLeft].Position;		// 5
			//const CameraSpacePoint &wristLeftPos		= joints[JointType_WristLeft].Position;		// 6
			//const CameraSpacePoint &shoulderRightPos	= joints[JointType_ShoulderRight].Position; // 8
			//const CameraSpacePoint &elbowRightPos		= joints[JointType_ElbowRight].Position;	// 9
			//const CameraSpacePoint &wristRightPos		= joints[JointType_WristRight].Position;	// 10
			const CameraSpacePoint &spineShoulderPos	= joints[JointType_SpineShoulder].Position; // 20
			
			
			// convert to centimeters
			
			double spineBase[3]		= { spineBasePos.X, spineBasePos.Y, spineBasePos.Z };
			double spineMid[3]		= { spineMidPos.X, spineMidPos.Y, spineMidPos.Z };
			double neck[3]			= { neckPos.X, neckPos.Y, neckPos.Z };
			double head[3]			= { headPos.X, headPos.Y, headPos.Z };
			//double shoulderLeft[3]	= { shoulderLeftPos.X, shoulderLeftPos.Y, shoulderLeftPos.Z };
			//double elbowLeft[3]		= { elbowLeftPos.X, elbowLeftPos.Y, elbowLeftPos.Z };
			//double wristLeft[3]		= { wristLeftPos.X, wristLeftPos.Y, wristLeftPos.Z };
			//double shoulderRight[3] = { shoulderRightPos.X, shoulderRightPos.Y, shoulderRightPos.Z };
			//double elbowRight[3]	= { elbowRightPos.X, elbowRightPos.Y, elbowRightPos.Z };
			//double wristRight[3]	= { wristRightPos.X, wristRightPos.Y, wristRightPos.Z };
			double spineShoulder[3] = { spineShoulderPos.X, spineShoulderPos.Y, spineShoulderPos.Z };

			// convert to centimeters
			int convFactor = 100;
			for (int i = 0; i < 3; i++) {
				spineBase[i]		= spineBase[i]		* convFactor;
				spineMid[i]			= spineMid[i]		* convFactor;
				neck[i]				= neck[i]			* convFactor;
				head[i]				= head[i]			* convFactor;
				//shoulderLeft[i]		= shoulderLeft[i]	* convFactor;
				//elbowLeft[i]		= elbowLeft[i]		* convFactor;
				//wristLeft[i]		= wristLeft[i]		* convFactor;
				//shoulderRight[i]	= shoulderRight[i]	* convFactor;
				//elbowRight[i]		= elbowRight[i]		* convFactor;
				//wristRight[i]		= wristRight[i]		* convFactor;
				spineShoulder[i]	= spineShoulder[i]	* convFactor;
			}
			


			// form differences

			double faceDV[3];
			double upperBodyDV[3];
			double lowerBodyDV[3];
			double neckDV[3];
			//double leftUpperArmDV[3];
			//double rightUpperArmDV[3];
			//double leftLowerArmDV[3];
			//double rightLowerArmDV[3];
			
			for (int i = 0; i < 3; i++) {
				faceDV[i]			= head[i] - neck[i];
				upperBodyDV[i]		= spineShoulder[i] - spineMid[i];
				lowerBodyDV[i]		= spineMid[i] - spineBase[i];
				neckDV[i]			= neck[i] - spineShoulder[i];
				//leftUpperArmDV[i]	= shoulderLeft[i] - elbowLeft[i];
				//rightUpperArmDV[i]	= shoulderRight[i] - elbowRight[i];
				//leftLowerArmDV[i]	= elbowLeft[i] - wristLeft[i];
				//rightLowerArmDV[i]	= elbowRight[i] - wristRight[i];
			}

			double differences[NUM_DIFFS];
			std::string names[NUM_DIFFS] = { "faceLen\t\t", "upperBodyLen\t", "lowerBodyLen\t", "neckLen\t\t"/*, 
				"leftUpperArmLen\t", "rightUpperArmLen", "leftLowerArmLen\t", "rightLowerArmLen"*//* };
			/*
			differences[0] =  sqrt( pow(faceDV[0], 2)			+ pow(faceDV[1], 2)				+ pow(faceDV[2], 2)); // faceLen
			differences[1] =  sqrt( pow(upperBodyDV[0], 2)		+ pow(upperBodyDV[1], 2)		+ pow(upperBodyDV[2], 2)); //upperBodyLen
			differences[2] =  sqrt( pow(lowerBodyDV[0], 2)		+ pow(lowerBodyDV[1], 2)		+ pow(lowerBodyDV[2], 2)); //lowerBodyLen
			differences[3] =  sqrt( pow(neckDV[0], 2)			+ pow(neckDV[1], 2)				+ pow(neckDV[2], 2)); //neckLen
			//differences[4] =  sqrt( pow(leftUpperArmDV[0], 2)	+ pow(leftUpperArmDV[1], 2)		+ pow(leftUpperArmDV[2], 2)); //leftUpperArmLen
			//differences[5] =  sqrt( pow(rightUpperArmDV[0], 2)	+ pow(rightUpperArmDV[1], 2)	+ pow(rightUpperArmDV[2], 2)); //rightUpperArmLen
			//differences[6] =  sqrt( pow(leftLowerArmDV[0], 2)	+ pow(leftLowerArmDV[1], 2)		+ pow(leftLowerArmDV[2], 2)); //leftLowerArmLen
			//differences[7] =  sqrt( pow(rightLowerArmDV[0], 2)	+ pow(rightLowerArmDV[1], 2)	+ pow(rightLowerArmDV[2], 2)); //rightLowerArmLen

			for (int i = 0; i < NUM_DIFFS; i++) {
				//double jointOfInterest = upperBodyLen;

				//differences[sampleCount % MAX_DIFFBUFF_SIZE] = jointOfInterest;
				if (differences[i] < lowestVal[i]) {
					lowestVal[i] = differences[i];
				}
				if (differences[i] > highestVal[i]) {
					highestVal[i] = differences[i];
				}

				//int endVal = MAX_DIFFBUFF_SIZE;
				//if (sampleCount < MAX_DIFFBUFF_SIZE) {
					//endVal = sampleCount;
				//}

				if (avgDiff[i] == -1) {
					std::cout << "Set average differences\n";					
					sampleCount = 0;
				}				

				avgDiff[i] = (avgDiff[i]*(sampleCount)+differences[i]) / (sampleCount + 1);

				/*
				double total = 0;
				for (int i = 0; i < endVal; i++) {
					total += differences[i];
				}
				double avgDiff = total / endVal; */
				/*
				std::cout << names[i].c_str() << "  " << avgDiff[i] /*<< "  " << lowestVal[i] << "  " << highestVal[i] *//* << "\n";
			}

			double avgFaceLen = avgDiff[0];
			double avgUpperBodyLen = avgDiff[1];
			double avgLowerBodyLen = avgDiff[2];
			double avgNeckLen = avgDiff[3];

			bool isFace = false;
			bool isUpperBody = false;
			bool isLowerBody = false;
			bool isNeck = false;


			if (avgFaceLen > jw.faceLen - jw.faceError && avgFaceLen < jw.faceLen + jw.faceError) {				
				isFace = true;
				std::cout << "Josiah's Face\n";
			}
			if (avgUpperBodyLen > jw.upperBodyLen - jw.upperBodyError && avgUpperBodyLen < jw.upperBodyLen + jw.upperBodyError) {
				isUpperBody = true;
				std::cout << "Josiah's Upper Body\n";
			}
			if (avgLowerBodyLen > jw.lowerBodyLen - jw.lowerBodyError && avgLowerBodyLen < jw.lowerBodyLen + jw.lowerBodyError) {
				isLowerBody = true;
				std::cout << "Josiah's Lower Body\n";
			}
			if (avgNeckLen > jw.neckLen - jw.neckError && avgNeckLen < jw.neckLen + jw.neckError) {
				isNeck = true;
				std::cout << "Josiah's Neck\n";
			}


			if (isFace && isUpperBody && isLowerBody && isNeck) {
				std::cout << "THIS IS JOSIAH!";
			}


			


			sampleCount++; */

			const CameraSpacePoint &spineMidPos = joints[JointType_SpineMid].Position;		// 1
			
			double spm_x = spineMidPos.X * 100;
			double spm_y = spineMidPos.Y * 100;
			double spm_z = spineMidPos.Z * 100;

			std::cout << "spineMid (x,y,z): (" << 
				spm_x << ", " << spm_y << ", " << spm_z << ")\n";

			bool moving = false;
			bool forward = false;
			float xThresh = 10.0;
			float zClose = 100.0;
			float zFar = 150.0;


			if (spm_z > zFar) {
				if (moving) {
					std::cout << "User too far away: SPEED UP";
					moving = true;
					forward = true;
				}
				else {
					std::cout << "User too far away: MOVE TOWARD USER";
					moving = true;
					forward = true;
				}
			}
			else if (spm_z < zClose) {
				if (moving) {
					std::cout << "User too close: SLOW DOWN";
					moving = true;
					forward = true;
				}
				else {
					std::cout << "User too close: BACK AWAY FROM USER";
					moving = true;
					forward = false;
				}
			}
			else {
				std::cout << "STOP";
				moving = false;				
			}

			if (spm_x > xThresh) {
				if (moving) {
					if (forward) {
						std::cout << " | VEER LEFT";
					}
					else {
						std::cout << " | BACKUP RIGHT";
					}
				}
				else {
					std::cout << " | TURN LEFT";
				}
			}
			else if (spm_x < -xThresh) {
				if (moving) {
					if (forward) {
						std::cout << " | VEER RIGHT";
					}
					else {
						std::cout << " | BACKUP LEFT";
					}
				}
				else {
					std::cout << " | TURN RIGHT";
				}
			}
			std::cout << "\n";



			// Figure out how to store data for use later.

			// Create color image with body tracking overlayed and position and depth of spine.			

        }
    }
}