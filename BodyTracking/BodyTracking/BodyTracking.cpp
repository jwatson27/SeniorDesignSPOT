#include <Kinect.h>
#include <iostream>

void processBodies(const unsigned int &bodyCount, IBody **bodies);

template<class Interface>
static inline void safeRelease(Interface *&interfaceToRelease)
{
    if (interfaceToRelease != nullptr) {
        interfaceToRelease->Release();
        interfaceToRelease = nullptr;
    }
}

int main(int argc, char *argv[])
{
    IKinectSensor *sensor = nullptr;
    IBodyFrameReader *bodyFrameReader = nullptr;

	// For MultiSourceFrames
	IMultiSourceFrameReader *multiSourceFrameReader = nullptr;
			


    //Get the default Kinect sensor
    HRESULT hr = GetDefaultKinectSensor(&sensor);

    //If the function succeeds, open the sensor
	if (SUCCEEDED(hr)) {
		hr = sensor->Open();
		
		/*
		if (SUCCEEDED(hr)) {
			//Get a body frame source from which we can get our body frame reader
			IBodyFrameSource *bodyFrameSource = nullptr;
			hr = sensor->get_BodyFrameSource(&bodyFrameSource);

			if (SUCCEEDED(hr)) {
				hr = bodyFrameSource->OpenReader(&bodyFrameReader);
			}

			//We're done with bodyFrameSource, so we'll release it
			safeRelease(bodyFrameSource);
		}

		/*/
        if (SUCCEEDED(hr)) {
            //Get a body frame source from which we can get our body frame reader
			hr = sensor->OpenMultiSourceFrameReader(FrameSourceTypes_Color    | FrameSourceTypes_Depth |
													FrameSourceTypes_Infrared | FrameSourceTypes_Body, 
													&multiSourceFrameReader);

			


			/*
			if (SUCCEEDED(hr)) {
				IMultiSourceFrame *multiSourceFrame = nullptr;
				
				
				while (FAILED(hr = multiSourceFrameReader->AcquireLatestFrame(&multiSourceFrame)));


				///IBodyFrameSource *bodyFrameSource = nullptr;
				///hr = sensor->get_BodyFrameSource(&bodyFrameSource);

				if (SUCCEEDED(hr)) {
					hr = multiSourceFrame->get_BodyFrameReference(&bodyFrameReference);
					///hr = bodyFrameSource->OpenReader(&bodyFrameReader);
				}

				//We're done with bodyFrameSource, so we'll release it
				///safeRelease(bodyFrameSource);
				safeRelease(multiSourceFrame);
			}*/
        }//*/
    }

    if (sensor == nullptr || FAILED(hr)) {
        std::cerr << "Cannot find any sensors.\n";
        return E_FAIL;
    }

    /*
	while (bodyFrameReader != nullptr) {
	/*/
	while (multiSourceFrameReader != nullptr) {
	//*/
		IMultiSourceFrame *multiSourceFrame = nullptr;
		IBodyFrameReference *bodyFrameReference = nullptr;
		IBodyFrame *bodyFrame = nullptr;
		IColorFrameReference *colorFrameReference = nullptr;
		IColorFrame *colorFrame = nullptr;

		if (SUCCEEDED(hr)) {						
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


			/*

			// Get Color Frame
			hr = multiSourceFrame->get_ColorFrameReference(&colorFrameReference);
			
			if (SUCCEEDED(hr)) {
				hr = colorFrameReference->AcquireFrame(&colorFrame);

				if (SUCCEEDED(hr)) {
					// Process Color Frame
					/*
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
						safeRelease(colorFrame);
					}
					*/
				/*}
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


			*/
						
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
        if (FAILED(hr) || isTracked == false) {
            continue;
        }

		std::cout << "Body Index: " << bodyIndex << " \n";

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


			const CameraSpacePoint &spineMidPos = joints[JointType_SpineMid].Position;
			
			// convert to centimeters
			float x = spineMidPos.X * pow(10, 2);
			float y = spineMidPos.Y * pow(10, 2);
			float z = spineMidPos.Z * pow(10, 2);

			std::cout << "spineMid (x,y,z): (" << 
				x << ", " << y << ", " << z << ")\n";

			bool moving = false;
			bool forward = false;
			float xThresh = 10.0;
			float zClose = 100.0;
			float zFar = 150.0;


			if (z > zFar) {
				if (moving) {
					std::cout << "Subject too far away: SPEED UP";
					moving = true;
					forward = true;
				}
				else {
					std::cout << "Subject too far away: MOVE TOWARD SUBJECT";
					moving = true;
					forward = true;
				}
			}
			else if (z < zClose) {
				if (moving) {
					std::cout << "Subject too close: SLOW DOWN";
					moving = true;
					forward = true;
				}
				else {
					std::cout << "Subject too close: BACK AWAY FROM SUBJECT";
					moving = true;
					forward = false;
				}
			}
			else {
				std::cout << "STOP";
				moving = false;				
			}

			if (x > xThresh) {
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
			else if (x < -xThresh) {
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