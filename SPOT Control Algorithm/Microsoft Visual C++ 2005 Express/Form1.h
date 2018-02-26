#pragma once
/********************************************************************
 FileName:		Form1.h
 Dependencies:	Windows Server 2003 R2 Platform SDK during development.  
				When compiled, needs .NET framework 2.0 redistributable to run.
				May also need MS VC++ redistributable to run.
 Hardware:		Need a free USB port to connect USB peripheral device
				programmed with appropriate Generic HID firmware.  VID and
				PID in firmware must match the VID and PID in this
				program.
 Compiler:  	Microsoft Visual C++ 2005 Express Edition (or better)
 Company:		Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the “Company”) for its PIC® Microcontroller is intended and
 supplied to you, the Company’s customer, for use solely and
 exclusively with Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************
 File Description:

 Change History:
  Rev   Date         Description
  2.3	08/28/2008	 Initial Release
********************************************************************
NOTE:	All user made code contained in this project is in the Form1.h file.
		All other code and files were generated automatically by either the
		new project wizard, or by the development environment (ex: code is
		automatically generated if you create a new button on the form, and
		then double click on it, which creates a click event handler
		function).  All user made code is contained in clearly marked cut and
		paste blocks.
********************************************************************/


// Kinect / OpenCV Include Files
#include <Kinect.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


// Global images
cv::Mat colorImage;
cv::Mat depthImage_short;
cv::Mat depthImage_byte;


// Function for cleaning up memory associated with Kinect objects
template<class Interface>
static inline void safeRelease(Interface *&interfaceToRelease)
{
	if (interfaceToRelease != nullptr) {
		interfaceToRelease->Release();
		interfaceToRelease = nullptr;
	}
}




// Form1 Includes
#include <Windows.h>	//Definitions for various common and not so common types like DWORD, PCHAR, HANDLE, etc.
#include <Dbt.h>		//Need this for definitions of WM_DEVICECHANGE messages
#include <setupapi.h>	//From Windows Server 2003 R2 Platform SDK.  Untested with Windows SDK.  Setupapi.h provides
						//definitions needed for the SetupDixxx() functions, which we use to find our plug and 
						//play device.  If getting build errors when trying to compile this project, make sure the
						//platform SDK is correctly installed, and that it has been integrated into the VC++ development
						//environment.  In other words, make sure the include paths have been setup correctly in this IDE.
						//Microsoft created some small tutorial videos showing how to do this.
						//These may be located at:
						//http://msdn.microsoft.com/en-us/visualc/aa336415.aspx 
						//The link is functional as of 21 May 2008, but could move.  If the above link does not work,
						//try searching for "Video1_PSDK.wmv", which may be associated with:
						//"Using Visual C++ Express Edition and the Platform SDK"

//Modify this value to match the VID and PID in your USB device descriptor (in the USB firmware on the microcontroller).
//Use the formatting: "Vid_xxxx&Pid_xxxx" where xxxx is a 16-bit hexadecimal number.
#define MY_DEVICE_ID  "Vid_04d8&Pid_005e"



namespace HIDPnPDemo {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;


#pragma region DLL Imports
	using namespace System::Threading;	
	using namespace System::Runtime::InteropServices;  //Need this to support "unmanaged" code.

	/*
	In order to use these unmanaged functions from within the managed .NET environment, we need
	to explicitly import the functions which we will be using from other .DLL file(s).  Simply
	including the appropriate header files is not enough. 

	Note: In order to avoid potential name conflicts in the header files (which we still use),
	I have renamed the functions by adding "UM" (unmanaged) onto the end of them.  To find 
	documentation for the functions in MSDN, search for the function name without the extra 
	"UM" attached.
	Note2: In the header files (such as setupapi.h), normally the function names are 
	remapped, depending upon if UNICODE is defined or not.  For example, two versions of the
	function SetupDiGetDeviceInterfaceDetail() exist.  One for UNICODE, and one for ANSI.  
	If the wrong version of the function is called, things won't work correctly.  Therefore,
	in order to make sure the correct one gets called (based on your compiler settings, which
	may or may not define "UNICODE"), it is useful to explicity specify the CharSet when doing
	the DLL import.
	*/

	#ifdef UNICODE
	#define	Seeifdef	Unicode
	#else
	#define Seeifdef	Ansi
	#endif

	//Returns a HDEVINFO type for a device information set (WinUSB devices in
	//our case).  We will need the HDEVINFO as in input parameter for calling many of
	//the other SetupDixxx() functions.
	[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiGetClassDevs")]		
	extern "C" HDEVINFO  SetupDiGetClassDevsUM(
		LPGUID  ClassGuid,					//Input: Supply the class GUID here. 
		PCTSTR  Enumerator,					//Input: Use NULL here, not important for our purposes
		HWND  hwndParent,					//Input: Use NULL here, not important for our purposes
		DWORD  Flags);						//Input: Flags describing what kind of filtering to use.

	//Gives us "PSP_DEVICE_INTERFACE_DATA" which contains the Interface specific GUID (different
	//from class GUID).  We need the interface GUID to get the device path.
	[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiEnumDeviceInterfaces")]				
	extern "C" WINSETUPAPI BOOL WINAPI  SetupDiEnumDeviceInterfacesUM(
		HDEVINFO  DeviceInfoSet,			//Input: Give it the HDEVINFO we got from SetupDiGetClassDevs()
		PSP_DEVINFO_DATA  DeviceInfoData,	//Input (optional)
		LPGUID  InterfaceClassGuid,			//Input 
		DWORD  MemberIndex,					//Input: "Index" of the device you are interested in getting the path for.
		PSP_DEVICE_INTERFACE_DATA  DeviceInterfaceData);//Output: This function fills in an "SP_DEVICE_INTERFACE_DATA" structure.

	//SetupDiDestroyDeviceInfoList() frees up memory by destroying a DeviceInfoList
	[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiDestroyDeviceInfoList")]
	extern "C" WINSETUPAPI BOOL WINAPI  SetupDiDestroyDeviceInfoListUM(			
		HDEVINFO  DeviceInfoSet);			//Input: Give it a handle to a device info list to deallocate from RAM.

	//SetupDiEnumDeviceInfo() fills in an "SP_DEVINFO_DATA" structure, which we need for SetupDiGetDeviceRegistryProperty()
	[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiEnumDeviceInfo")]
	extern "C" WINSETUPAPI BOOL WINAPI  SetupDiEnumDeviceInfoUM(
		HDEVINFO  DeviceInfoSet,
		DWORD  MemberIndex,
		PSP_DEVINFO_DATA  DeviceInfoData);

	//SetupDiGetDeviceRegistryProperty() gives us the hardware ID, which we use to check to see if it has matching VID/PID
	[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiGetDeviceRegistryProperty")]
	extern "C"	WINSETUPAPI BOOL WINAPI  SetupDiGetDeviceRegistryPropertyUM(
		HDEVINFO  DeviceInfoSet,
		PSP_DEVINFO_DATA  DeviceInfoData,
		DWORD  Property,
		PDWORD  PropertyRegDataType,
		PBYTE  PropertyBuffer,   
		DWORD  PropertyBufferSize,  
		PDWORD  RequiredSize);

	//SetupDiGetDeviceInterfaceDetail() gives us a device path, which is needed before CreateFile() can be used.
	[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiGetDeviceInterfaceDetail")]
	extern "C" BOOL SetupDiGetDeviceInterfaceDetailUM(
		HDEVINFO DeviceInfoSet,										//Input: Wants HDEVINFO which can be obtained from SetupDiGetClassDevs()
		PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,				//Input: Pointer to an structure which defines the device interface.  
		PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData,	//Output: Pointer to a strucutre, which will contain the device path.
		DWORD DeviceInterfaceDetailDataSize,						//Input: Number of bytes to retrieve.
		PDWORD RequiredSize,										//Output (optional): Te number of bytes needed to hold the entire struct 
		PSP_DEVINFO_DATA DeviceInfoData);							//Output

	//Need this function for receiving all of the WM_DEVICECHANGE messages.  See MSDN documentation for
	//description of what this function does/how to use it. Note: name is remapped "RegisterDeviceNotificationUM" to
	//avoid possible build error conflicts.
	[DllImport("user32.dll" , CharSet = CharSet::Seeifdef, EntryPoint="RegisterDeviceNotification")]					
	extern "C" HDEVNOTIFY WINAPI RegisterDeviceNotificationUM(
		HANDLE hRecipient,
		LPVOID NotificationFilter,
		DWORD Flags);

#pragma endregion




#pragma region Global Variables
	/*** This section is all of the global variables related to this namespace ***/

	//Globally Unique Identifier (GUID) for HID class devices.  Windows uses GUIDs to identify things.
	GUID InterfaceClassGuid = {0x4d1e55b2, 0xf16f, 0x11cf, 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}; 

	//USB related variables that need to have wide scope.
	BOOL AttachedState = FALSE;						//Need to keep track of the USB device attachment status for proper plug and play operation.
	BOOL AttachedButBroken = FALSE;					
	PSP_DEVICE_INTERFACE_DETAIL_DATA DetailedInterfaceDataStructure = new SP_DEVICE_INTERFACE_DETAIL_DATA;	//Global
	HANDLE WriteHandleToUSBDevice = INVALID_HANDLE_VALUE;
	HANDLE ReadHandleToUSBDevice = INVALID_HANDLE_VALUE;

	//Variables used by the application/form updates.
	BOOL PushbuttonPressed = FALSE;				//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)
	BOOL mTouchCalibratePending = FALSE;		//Updated by ToggleLED(s) button click event handler, used by ReadWriteThread (needs to be atomic)
	unsigned int ADCValueL = 0;					//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)
	unsigned int ADCValueR = 0;					//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)
	unsigned int ADCValueSliderTop = 0;			//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)
	unsigned int ADCValueSliderBottom = 0;		//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)
	
	signed int ACCValueX = 0;				// Changed these types from signed char to signed int to display larger numbers for debugging
	signed int ACCValueY = 0;
	//signed char ACCValueX = 0;					//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)
	//signed char ACCValueY = 0;					//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)
	signed char ACCValueZ = 0;					//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)
	unsigned char ACCValueT = 0;				//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)

	unsigned int ADCValuePot = 0;				//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)

	unsigned int maxADCValueL = 0;					//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)
	unsigned int maxADCValueR = 0;					//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)
	unsigned int maxADCValueSliderTop = 0;			//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)
	unsigned int maxADCValueSliderBottom = 0;		//Updated by ReadWriteThread, read by FormUpdateTimer tick handler (needs to be atomic)

#pragma endregion




	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form {
		public:
			Form1(void) {
				InitializeComponent();
				//Additional constructor code

				//Register for WM_DEVICECHANGE notifications.  This code uses these messages to detect plug and play connection/disconnection events for USB devices
				DEV_BROADCAST_DEVICEINTERFACE MyDeviceBroadcastHeader;// = new DEV_BROADCAST_HDR;
				MyDeviceBroadcastHeader.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
				MyDeviceBroadcastHeader.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
				MyDeviceBroadcastHeader.dbcc_reserved = 0;	//Reserved says not to use...
				MyDeviceBroadcastHeader.dbcc_classguid = InterfaceClassGuid;
				RegisterDeviceNotificationUM((HANDLE)this->Handle, &MyDeviceBroadcastHeader, DEVICE_NOTIFY_WINDOW_HANDLE);

				//Now make an initial attempt to find the USB device, if it was already connected to the PC and enumerated prior to launching the application.
				//If it is connected and present, we should open read and write handles to the device so we can communicate with it later.
				//If it was not connected, we will have to wait until the user plugs the device in, and the WM_DEVICECHANGE callback function can process
				//the message and again search for the device.
				if(CheckIfPresentAndGetUSBDevicePath())	{ //Check and make sure at least one device with matching VID/PID is attached
					DWORD ErrorStatusWrite;
					DWORD ErrorStatusRead;

					//We now have the proper device path, and we can finally open read and write handles to the device.
					WriteHandleToUSBDevice = CreateFile(DetailedInterfaceDataStructure->DevicePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
					ErrorStatusWrite = GetLastError();
					ReadHandleToUSBDevice = CreateFile(DetailedInterfaceDataStructure->DevicePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
					ErrorStatusRead = GetLastError();

					if((ErrorStatusWrite == ERROR_SUCCESS) && (ErrorStatusRead == ERROR_SUCCESS)) {
						AttachedState = TRUE;		//Let the rest of the PC application know the USB device is connected, and it is safe to read/write to it
						AttachedButBroken = FALSE;
					}
					else { //for some reason the device was physically plugged in, but one or both of the read/write handles didn't open successfully...
						AttachedState = FALSE;		//Let the rest of this application known not to read/write to the device.
						AttachedButBroken = TRUE;	//Flag so that next time a WM_DEVICECHANGE message occurs, can retry to re-open read/write pipes
						if(ErrorStatusWrite == ERROR_SUCCESS)
							CloseHandle(WriteHandleToUSBDevice);
						if(ErrorStatusRead == ERROR_SUCCESS)
							CloseHandle(ReadHandleToUSBDevice);
					}
				}
				else {	//Device must not be connected (or not programmed with correct firmware)
					AttachedState = FALSE;
					AttachedButBroken = FALSE;
				}


				ReadWriteThread->RunWorkerAsync();	//Recommend performing USB read/write operations in a separate thread.  Otherwise,
													//the Read/Write operations are effectively blocking functions and can lock up the
													//user interface if the I/O operations take a long time to complete.
			}

		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			~Form1() {
				//Close the read/write handles, if they are currently open.
				if(AttachedState == TRUE) {
					CloseHandle(WriteHandleToUSBDevice);
					CloseHandle(ReadHandleToUSBDevice);
				}
			
				if (components) {
					delete components;
				}
			}


		private: System::Windows::Forms::TextBox^  StatusBox_txtbx;
		private: System::Windows::Forms::ProgressBar^  progressBarL;

		private: System::ComponentModel::BackgroundWorker^  ReadWriteThread;
		private: System::Windows::Forms::Timer^  FormUpdateTimer;
			 
		private: System::Windows::Forms::ProgressBar^  progressBarR;
		private: System::Windows::Forms::ProgressBar^  progressBarSliderBottom;

		private: System::Windows::Forms::ProgressBar^  progressBarSliderTop;
		private: System::Windows::Forms::TabControl^  tabControl1;
		private: System::Windows::Forms::TabPage^  Graphical;

		private: System::Windows::Forms::TabPage^  tabPage2;
		private: System::Windows::Forms::Label^  label4;
		private: System::Windows::Forms::Label^  label3;
		private: System::Windows::Forms::Label^  label2;
		private: System::Windows::Forms::Label^  label1;
		private: System::Windows::Forms::NumericUpDown^  numericUpDown4;
		private: System::Windows::Forms::NumericUpDown^  numericUpDown3;
		private: System::Windows::Forms::NumericUpDown^  numericUpDown2;
		private: System::Windows::Forms::NumericUpDown^  numericUpDown1;
		private: System::Windows::Forms::Button^  button1;
		private: System::Windows::Forms::PictureBox^  pictureBox1;
		private: System::Windows::Forms::PictureBox^  pictureBox2;
		private: System::Windows::Forms::NumericUpDown^  numericUpDown7;
		private: System::Windows::Forms::NumericUpDown^  numericUpDown6;
		private: System::Windows::Forms::NumericUpDown^  numericUpDown5;
		private: System::Windows::Forms::Label^  label5;
		private: System::Windows::Forms::Label^  label7;
		private: System::Windows::Forms::Label^  label6;
		private: System::Windows::Forms::Label^  label8;
		private: System::Windows::Forms::NumericUpDown^  numericUpDown8;
		private: System::Windows::Forms::Label^  label9;
		private: System::Windows::Forms::NumericUpDown^  numericUpDown9;

		private: System::ComponentModel::IContainer^  components;

		private:
			/// <summary>
			/// Required designer variable.
			/// </summary>


		#pragma region Windows Form Designer generated code
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			void InitializeComponent(void) {
				this->components = (gcnew System::ComponentModel::Container());
				System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
				this->StatusBox_txtbx = (gcnew System::Windows::Forms::TextBox());
				this->progressBarL = (gcnew System::Windows::Forms::ProgressBar());
				this->ReadWriteThread = (gcnew System::ComponentModel::BackgroundWorker());
				this->FormUpdateTimer = (gcnew System::Windows::Forms::Timer(this->components));
				this->progressBarR = (gcnew System::Windows::Forms::ProgressBar());
				this->progressBarSliderBottom = (gcnew System::Windows::Forms::ProgressBar());
				this->progressBarSliderTop = (gcnew System::Windows::Forms::ProgressBar());
				this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
				this->Graphical = (gcnew System::Windows::Forms::TabPage());
				this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
				this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
				this->label8 = (gcnew System::Windows::Forms::Label());
				this->label7 = (gcnew System::Windows::Forms::Label());
				this->label6 = (gcnew System::Windows::Forms::Label());
				this->label5 = (gcnew System::Windows::Forms::Label());
				this->numericUpDown8 = (gcnew System::Windows::Forms::NumericUpDown());
				this->numericUpDown7 = (gcnew System::Windows::Forms::NumericUpDown());
				this->numericUpDown6 = (gcnew System::Windows::Forms::NumericUpDown());
				this->numericUpDown5 = (gcnew System::Windows::Forms::NumericUpDown());
				this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
				this->button1 = (gcnew System::Windows::Forms::Button());
				this->label4 = (gcnew System::Windows::Forms::Label());
				this->label3 = (gcnew System::Windows::Forms::Label());
				this->label2 = (gcnew System::Windows::Forms::Label());
				this->label1 = (gcnew System::Windows::Forms::Label());
				this->numericUpDown4 = (gcnew System::Windows::Forms::NumericUpDown());
				this->numericUpDown3 = (gcnew System::Windows::Forms::NumericUpDown());
				this->numericUpDown2 = (gcnew System::Windows::Forms::NumericUpDown());
				this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
				this->numericUpDown9 = (gcnew System::Windows::Forms::NumericUpDown());
				this->label9 = (gcnew System::Windows::Forms::Label());
				this->tabControl1->SuspendLayout();
				this->Graphical->SuspendLayout();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
				this->tabPage2->SuspendLayout();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown8))->BeginInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown7))->BeginInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown6))->BeginInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown5))->BeginInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->BeginInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown4))->BeginInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown3))->BeginInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown2))->BeginInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->BeginInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown9))->BeginInit();
				this->SuspendLayout();
				// 
				// StatusBox_txtbx
				// 
				this->StatusBox_txtbx->BackColor = System::Drawing::SystemColors::Window;
				this->StatusBox_txtbx->Location = System::Drawing::Point(12, 10);
				this->StatusBox_txtbx->Multiline = true;
				this->StatusBox_txtbx->Name = L"StatusBox_txtbx";
				this->StatusBox_txtbx->ReadOnly = true;
				this->StatusBox_txtbx->Size = System::Drawing::Size(193, 43);
				this->StatusBox_txtbx->TabIndex = 15;
				this->StatusBox_txtbx->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
				// 
				// progressBarL
				// 
				this->progressBarL->BackColor = System::Drawing::Color::White;
				this->progressBarL->ForeColor = System::Drawing::Color::Red;
				this->progressBarL->Location = System::Drawing::Point(22, 20);
				this->progressBarL->Maximum = 1024;
				this->progressBarL->Name = L"progressBarL";
				this->progressBarL->Size = System::Drawing::Size(60, 60);
				this->progressBarL->Step = 1;
				this->progressBarL->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->progressBarL->TabIndex = 14;
				// 
				// ReadWriteThread
				// 
				this->ReadWriteThread->WorkerReportsProgress = true;
				this->ReadWriteThread->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::ReadWriteThread_DoWork);
				// 
				// FormUpdateTimer
				// 
				this->FormUpdateTimer->Enabled = true;
				this->FormUpdateTimer->Interval = 6;
				this->FormUpdateTimer->Tick += gcnew System::EventHandler(this, &Form1::FormUpdateTimer_Tick);
				// 
				// progressBarR
				// 
				this->progressBarR->BackColor = System::Drawing::Color::White;
				this->progressBarR->ForeColor = System::Drawing::Color::Red;
				this->progressBarR->Location = System::Drawing::Point(102, 20);
				this->progressBarR->Maximum = 1024;
				this->progressBarR->Name = L"progressBarR";
				this->progressBarR->Size = System::Drawing::Size(60, 60);
				this->progressBarR->Step = 1;
				this->progressBarR->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->progressBarR->TabIndex = 14;
				// 
				// progressBarSliderBottom
				// 
				this->progressBarSliderBottom->BackColor = System::Drawing::Color::White;
				this->progressBarSliderBottom->ForeColor = System::Drawing::Color::Red;
				this->progressBarSliderBottom->Location = System::Drawing::Point(22, 100);
				this->progressBarSliderBottom->Maximum = 1024;
				this->progressBarSliderBottom->Name = L"progressBarSliderBottom";
				this->progressBarSliderBottom->Size = System::Drawing::Size(140, 20);
				this->progressBarSliderBottom->Step = 1;
				this->progressBarSliderBottom->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->progressBarSliderBottom->TabIndex = 14;
				// 
				// progressBarSliderTop
				// 
				this->progressBarSliderTop->BackColor = System::Drawing::Color::White;
				this->progressBarSliderTop->ForeColor = System::Drawing::Color::Red;
				this->progressBarSliderTop->Location = System::Drawing::Point(22, 130);
				this->progressBarSliderTop->Maximum = 1024;
				this->progressBarSliderTop->Name = L"progressBarSliderTop";
				this->progressBarSliderTop->Size = System::Drawing::Size(140, 20);
				this->progressBarSliderTop->Step = 1;
				this->progressBarSliderTop->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
				this->progressBarSliderTop->TabIndex = 14;
				// 
				// tabControl1
				// 
				this->tabControl1->Controls->Add(this->Graphical);
				this->tabControl1->Controls->Add(this->tabPage2);
				this->tabControl1->Location = System::Drawing::Point(12, 59);
				this->tabControl1->Name = L"tabControl1";
				this->tabControl1->SelectedIndex = 0;
				this->tabControl1->Size = System::Drawing::Size(193, 427);
				this->tabControl1->TabIndex = 16;
				// 
				// Graphical
				// 
				this->Graphical->Controls->Add(this->pictureBox1);
				this->Graphical->Controls->Add(this->progressBarL);
				this->Graphical->Controls->Add(this->progressBarSliderBottom);
				this->Graphical->Controls->Add(this->progressBarR);
				this->Graphical->Controls->Add(this->progressBarSliderTop);
				this->Graphical->Location = System::Drawing::Point(4, 22);
				this->Graphical->Name = L"Graphical";
				this->Graphical->Padding = System::Windows::Forms::Padding(3);
				this->Graphical->Size = System::Drawing::Size(185, 401);
				this->Graphical->TabIndex = 0;
				this->Graphical->Text = L"Visual";
				this->Graphical->UseVisualStyleBackColor = true;
				// 
				// pictureBox1
				// 
				this->pictureBox1->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
				this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.Image")));
				this->pictureBox1->Location = System::Drawing::Point(6, 346);
				this->pictureBox1->Name = L"pictureBox1";
				this->pictureBox1->Size = System::Drawing::Size(174, 49);
				this->pictureBox1->TabIndex = 15;
				this->pictureBox1->TabStop = false;
				// 
				// tabPage2
				// 
				this->tabPage2->Controls->Add(this->label9);
				this->tabPage2->Controls->Add(this->label8);
				this->tabPage2->Controls->Add(this->label7);
				this->tabPage2->Controls->Add(this->label6);
				this->tabPage2->Controls->Add(this->label5);
				this->tabPage2->Controls->Add(this->numericUpDown9);
				this->tabPage2->Controls->Add(this->numericUpDown8);
				this->tabPage2->Controls->Add(this->numericUpDown7);
				this->tabPage2->Controls->Add(this->numericUpDown6);
				this->tabPage2->Controls->Add(this->numericUpDown5);
				this->tabPage2->Controls->Add(this->pictureBox2);
				this->tabPage2->Controls->Add(this->button1);
				this->tabPage2->Controls->Add(this->label4);
				this->tabPage2->Controls->Add(this->label3);
				this->tabPage2->Controls->Add(this->label2);
				this->tabPage2->Controls->Add(this->label1);
				this->tabPage2->Controls->Add(this->numericUpDown4);
				this->tabPage2->Controls->Add(this->numericUpDown3);
				this->tabPage2->Controls->Add(this->numericUpDown2);
				this->tabPage2->Controls->Add(this->numericUpDown1);
				this->tabPage2->Location = System::Drawing::Point(4, 22);
				this->tabPage2->Name = L"tabPage2";
				this->tabPage2->Padding = System::Windows::Forms::Padding(3);
				this->tabPage2->Size = System::Drawing::Size(185, 401);
				this->tabPage2->TabIndex = 1;
				this->tabPage2->Text = L"Numeric";
				this->tabPage2->UseVisualStyleBackColor = true;
				// 
				// label8
				// 
				this->label8->AutoSize = true;
				this->label8->Location = System::Drawing::Point(93, 273);
				this->label8->Name = L"label8";
				this->label8->Size = System::Drawing::Size(87, 13);
				this->label8->TabIndex = 18;
				this->label8->Text = L"Temperature (°C)";
				// 
				// label7
				// 
				this->label7->AutoSize = true;
				this->label7->Location = System::Drawing::Point(93, 233);
				this->label7->Name = L"label7";
				this->label7->Size = System::Drawing::Size(36, 13);
				this->label7->TabIndex = 18;
				this->label7->Text = L"Angle (deg Z/X)";
				// 
				// label6
				// 
				this->label6->AutoSize = true;
				this->label6->Location = System::Drawing::Point(93, 193);
				this->label6->Name = L"label6";
				this->label6->Size = System::Drawing::Size(36, 13);
				this->label6->TabIndex = 18;
				this->label6->Text = L"Position X (cm)";
				// 
				// label5
				// 
				this->label5->AutoSize = true;
				this->label5->Location = System::Drawing::Point(93, 153);
				this->label5->Name = L"label5";
				this->label5->Size = System::Drawing::Size(36, 13);
				this->label5->TabIndex = 18;
				this->label5->Text = L"Distance Z (cm)";
				// 
				// numericUpDown8
				// 
				this->numericUpDown8->Location = System::Drawing::Point(20, 270);
				this->numericUpDown8->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {30, 0, 0, System::Int32::MinValue});
				this->numericUpDown8->Name = L"numericUpDown8";
				this->numericUpDown8->Size = System::Drawing::Size(69, 20);
				this->numericUpDown8->TabIndex = 17;
				this->numericUpDown8->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {30, 0, 0, System::Int32::MinValue});
				// 
				// numericUpDown7
				// 
				this->numericUpDown7->Location = System::Drawing::Point(20, 230);
				this->numericUpDown7->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {256, 0, 0, 0});
				this->numericUpDown7->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {256, 0, 0, System::Int32::MinValue});
				this->numericUpDown7->Name = L"numericUpDown7";
				this->numericUpDown7->Size = System::Drawing::Size(69, 20);
				this->numericUpDown7->TabIndex = 17;
				// 
				// numericUpDown6
				// 
				this->numericUpDown6->Location = System::Drawing::Point(20, 190);
				this->numericUpDown6->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {500, 0, 0, 0});
				this->numericUpDown6->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {500, 0, 0, System::Int32::MinValue});
				this->numericUpDown6->Name = L"numericUpDown6";
				this->numericUpDown6->Size = System::Drawing::Size(69, 20);
				this->numericUpDown6->TabIndex = 17;
				// 
				// numericUpDown5
				// 
				this->numericUpDown5->Location = System::Drawing::Point(20, 150);
				this->numericUpDown5->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {500, 0, 0, 0});
				this->numericUpDown5->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {500, 0, 0, System::Int32::MinValue});
				this->numericUpDown5->Name = L"numericUpDown5";
				this->numericUpDown5->Size = System::Drawing::Size(69, 20);
				this->numericUpDown5->TabIndex = 17;
				// 
				// pictureBox2
				// 
				this->pictureBox2->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
				this->pictureBox2->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox2.Image")));
				this->pictureBox2->Location = System::Drawing::Point(6, 346);
				this->pictureBox2->Name = L"pictureBox2";
				this->pictureBox2->Size = System::Drawing::Size(174, 49);
				this->pictureBox2->TabIndex = 16;
				this->pictureBox2->TabStop = false;
				// 
				// button1
				// 
				this->button1->ForeColor = System::Drawing::Color::Red;
				this->button1->Location = System::Drawing::Point(20, 110);
				this->button1->Name = L"button1";
				this->button1->Size = System::Drawing::Size(146, 20);
				this->button1->TabIndex = 2;
				this->button1->Text = L"mTouchCalibrate";
				this->button1->UseVisualStyleBackColor = true;
				this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
				// 
				// label4
				// 
				this->label4->AutoSize = true;
				this->label4->Location = System::Drawing::Point(100, 55);
				this->label4->Name = L"label4";
				this->label4->Size = System::Drawing::Size(69, 13);
				this->label4->TabIndex = 1;
				this->label4->Text = L"Right Direction";
				// 
				// label3
				// 
				this->label3->AutoSize = true;
				this->label3->Location = System::Drawing::Point(20, 55);
				this->label3->Name = L"label3";
				this->label3->Size = System::Drawing::Size(55, 13);
				this->label3->TabIndex = 1;
				this->label3->Text = L"Left Direction";
				// 
				// label2
				// 
				this->label2->AutoSize = true;
				this->label2->Location = System::Drawing::Point(100, 5);
				this->label2->Name = L"label2";
				this->label2->Size = System::Drawing::Size(66, 13);
				this->label2->TabIndex = 1;
				this->label2->Text = L"Right Speed";
				// 
				// label1
				// 
				this->label1->AutoSize = true;
				this->label1->Location = System::Drawing::Point(20, 5);
				this->label1->Name = L"label1";
				this->label1->Size = System::Drawing::Size(59, 13);
				this->label1->TabIndex = 1;
				this->label1->Text = L"Left Speed";
				// 
				// numericUpDown4
				// 
				this->numericUpDown4->Location = System::Drawing::Point(100, 70);
				this->numericUpDown4->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1024, 0, 0, 0});
				this->numericUpDown4->Name = L"numericUpDown4";
				this->numericUpDown4->Size = System::Drawing::Size(69, 20);
				this->numericUpDown4->TabIndex = 0;
				// 
				// numericUpDown3
				// 
				this->numericUpDown3->Location = System::Drawing::Point(20, 70);
				this->numericUpDown3->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1024, 0, 0, 0});
				this->numericUpDown3->Name = L"numericUpDown3";
				this->numericUpDown3->Size = System::Drawing::Size(69, 20);
				this->numericUpDown3->TabIndex = 0;
				// 
				// numericUpDown2
				// 
				this->numericUpDown2->Location = System::Drawing::Point(100, 20);
				this->numericUpDown2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1024, 0, 0, 0});
				this->numericUpDown2->Name = L"numericUpDown2";
				this->numericUpDown2->Size = System::Drawing::Size(69, 20);
				this->numericUpDown2->TabIndex = 0;
				// 
				// numericUpDown1
				// 
				this->numericUpDown1->Location = System::Drawing::Point(20, 20);
				this->numericUpDown1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1024, 0, 0, 0});
				this->numericUpDown1->Name = L"numericUpDown1";
				this->numericUpDown1->Size = System::Drawing::Size(69, 20);
				this->numericUpDown1->TabIndex = 0;
				// 
				// numericUpDown9
				// 
				this->numericUpDown9->Location = System::Drawing::Point(20, 310);
				this->numericUpDown9->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1024, 0, 0, 0});
				this->numericUpDown9->Name = L"numericUpDown9";
				this->numericUpDown9->Size = System::Drawing::Size(69, 20);
				this->numericUpDown9->TabIndex = 17;
				// 
				// label9
				// 
				this->label9->AutoSize = true;
				this->label9->Location = System::Drawing::Point(93, 313);
				this->label9->Name = L"label9";
				this->label9->Size = System::Drawing::Size(72, 13);
				this->label9->TabIndex = 18;
				this->label9->Text = L"In Range";
				// 
				// Form1
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(218, 498);
				this->Controls->Add(this->tabControl1);
				this->Controls->Add(this->StatusBox_txtbx);
				this->Name = L"Form1";
				this->Text = L"Generic HID PnP Demo";
				this->tabControl1->ResumeLayout(false);
				this->Graphical->ResumeLayout(false);
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
				this->tabPage2->ResumeLayout(false);
				this->tabPage2->PerformLayout();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown8))->EndInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown7))->EndInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown6))->EndInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown5))->EndInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->EndInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown4))->EndInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown3))->EndInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown2))->EndInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->EndInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown9))->EndInit();
				this->ResumeLayout(false);
				this->PerformLayout();

			}
		#pragma endregion



		//FUNCTION:	CheckIfPresentAndGetUSBDevicePath()
		//PURPOSE:	Check if a USB device is currently plugged in with a matching VID and PID
		//INPUT:	Uses globally declared "DetailedInterfaceDataStructure" structure, globally declared GUID, and the MY_DEVICE_ID constant.
		//OUTPUT:	Returns BOOL.  TRUE when device with matching VID/PID found.  FALSE if device with VID/PID could not be found.
		//			When returns TRUE, the globally accessable "DetailedInterfaceDataStructure" will contain the device path
		//			to the USB device with the matching VID/PID.
		BOOL	CheckIfPresentAndGetUSBDevicePath(void) {
			/* 
			Before we can "connect" our application to our USB embedded device, we must first find the device.
			A USB bus can have many devices simultaneously connected, so somehow we have to find our device only.
			This is done with the Vendor ID (VID) and Product ID (PID).  Each USB product line should have
			a unique combination of VID and PID.  

			Microsoft has created a number of functions which are useful for finding plug and play devices.  Documentation
			for each function used can be found in the MSDN library.  We will be using the following functions:

			SetupDiGetClassDevs()					//provided by setupapi.dll, which comes with Windows
			SetupDiEnumDeviceInterfaces()			//provided by setupapi.dll, which comes with Windows
			GetLastError()							//provided by kernel32.dll, which comes with Windows
			SetupDiDestroyDeviceInfoList()			//provided by setupapi.dll, which comes with Windows
			SetupDiGetDeviceInterfaceDetail()		//provided by setupapi.dll, which comes with Windows
			SetupDiGetDeviceRegistryProperty()		//provided by setupapi.dll, which comes with Windows
			malloc()								//part of C runtime library, msvcrt.dll?
			CreateFile()							//provided by kernel32.dll, which comes with Windows

			We will also be using the following unusual data types and structures.  Documentation can also be found in
			the MSDN library:

			PSP_DEVICE_INTERFACE_DATA
			PSP_DEVICE_INTERFACE_DETAIL_DATA
			SP_DEVINFO_DATA
			HDEVINFO
			HANDLE
			GUID

			The ultimate objective of the following code is to get the device path, which will be used elsewhere for getting
			read and write handles to the USB device.  Once the read/write handles are opened, only then can this
			PC application begin reading/writing to the USB device using the WriteFile() and ReadFile() functions.

			Getting the device path is a multi-step round about process, which requires calling several of the
			SetupDixxx() functions provided by setupapi.dll.
			*/

			HDEVINFO DeviceInfoTable = INVALID_HANDLE_VALUE;
			PSP_DEVICE_INTERFACE_DATA InterfaceDataStructure = new SP_DEVICE_INTERFACE_DATA;
	//		PSP_DEVICE_INTERFACE_DETAIL_DATA DetailedInterfaceDataStructure = new SP_DEVICE_INTERFACE_DETAIL_DATA;	//Globally declared instead
			SP_DEVINFO_DATA DevInfoData;

			DWORD InterfaceIndex = 0;
			DWORD StatusLastError = 0;
			DWORD dwRegType;
			DWORD dwRegSize;
			DWORD StructureSize = 0;
			PBYTE PropertyValueBuffer;
			bool MatchFound = false;
			DWORD ErrorStatus;
			BOOL BoolStatus = FALSE;
			DWORD LoopCounter = 0;

			String^ DeviceIDToFind = MY_DEVICE_ID;

			//First populate a list of plugged in devices (by specifying "DIGCF_PRESENT"), which are of the specified class GUID. 
			DeviceInfoTable = SetupDiGetClassDevsUM(&InterfaceClassGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

			//Now look through the list we just populated.  We are trying to see if any of them match our device. 
			while(true) {
				InterfaceDataStructure->cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
				if(SetupDiEnumDeviceInterfacesUM(DeviceInfoTable, NULL, &InterfaceClassGuid, InterfaceIndex, InterfaceDataStructure)) {
					ErrorStatus = GetLastError();
					if(ERROR_NO_MORE_ITEMS == GetLastError()) {	//Did we reach the end of the list of matching devices in the DeviceInfoTable?
						//Cound not find the device.  Must not have been attached.
						SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
						return FALSE;		
					}
				}
				else {	//Else some other kind of unknown error ocurred...
					ErrorStatus = GetLastError();
					SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
					return FALSE;	
				}

				//Now retrieve the hardware ID from the registry.  The hardware ID contains the VID and PID, which we will then 
				//check to see if it is the correct device or not.

				//Initialize an appropriate SP_DEVINFO_DATA structure.  We need this structure for SetupDiGetDeviceRegistryProperty().
				DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
				SetupDiEnumDeviceInfoUM(DeviceInfoTable, InterfaceIndex, &DevInfoData);

				//First query for the size of the hardware ID, so we can know how big a buffer to allocate for the data.
				SetupDiGetDeviceRegistryPropertyUM(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, &dwRegType, NULL, 0, &dwRegSize);

				//Allocate a buffer for the hardware ID.
				PropertyValueBuffer = (BYTE *) malloc (dwRegSize);
				if(PropertyValueBuffer == NULL)	{ //if null, error, couldn't allocate enough memory
					//Can't really recover from this situation, just exit instead.
					SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
					return FALSE;		
				}

				//Retrieve the hardware IDs for the current device we are looking at.  PropertyValueBuffer gets filled with a 
				//REG_MULTI_SZ (array of null terminated strings).  To find a device, we only care about the very first string in the
				//buffer, which will be the "device ID".  The device ID is a string which contains the VID and PID, in the example 
				//format "Vid_04d8&Pid_003f".
				SetupDiGetDeviceRegistryPropertyUM(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, &dwRegType, PropertyValueBuffer, dwRegSize, NULL);

				//Now check if the first string in the hardware ID matches the device ID of my USB device.
				#ifdef UNICODE
				String^ DeviceIDFromRegistry = gcnew String((wchar_t *)PropertyValueBuffer);
				#else
				String^ DeviceIDFromRegistry = gcnew String((char *)PropertyValueBuffer);
				#endif

				free(PropertyValueBuffer);		//No longer need the PropertyValueBuffer, free the memory to prevent potential memory leaks

				//Convert both strings to lower case.  This makes the code more robust/portable accross OS Versions
				DeviceIDFromRegistry = DeviceIDFromRegistry->ToLowerInvariant();	
				DeviceIDToFind = DeviceIDToFind->ToLowerInvariant();				
				//Now check if the hardware ID we are looking at contains the correct VID/PID
				MatchFound = DeviceIDFromRegistry->Contains(DeviceIDToFind);		
				if(MatchFound == true) {
					//Device must have been found.  Open WinUSB interface handle now.  In order to do this, we will need the actual device path first.
					//We can get the path by calling SetupDiGetDeviceInterfaceDetail(), however, we have to call this function twice:  The first
					//time to get the size of the required structure/buffer to hold the detailed interface data, then a second time to actually 
					//get the structure (after we have allocated enough memory for the structure.)
					DetailedInterfaceDataStructure->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
					//First call populates "StructureSize" with the correct value
					SetupDiGetDeviceInterfaceDetailUM(DeviceInfoTable, InterfaceDataStructure, NULL, NULL, &StructureSize, NULL);	
					DetailedInterfaceDataStructure = (PSP_DEVICE_INTERFACE_DETAIL_DATA)(malloc(StructureSize));		//Allocate enough memory
					if(DetailedInterfaceDataStructure == NULL) { //if null, error, couldn't allocate enough memory
						//Can't really recover from this situation, just exit instead.
						SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
						return FALSE;		
					}
					DetailedInterfaceDataStructure->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
					 //Now call SetupDiGetDeviceInterfaceDetail() a second time to receive the goods.  
					SetupDiGetDeviceInterfaceDetailUM(DeviceInfoTable, InterfaceDataStructure, DetailedInterfaceDataStructure, StructureSize, NULL, NULL); 

					//We now have the proper device path, and we can finally open a device handle to the device.
					//WinUSB requires the device handle to be opened with the FILE_FLAG_OVERLAPPED attribute.
					SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
					return TRUE;
				}

				InterfaceIndex++;	
				//Keep looping until we either find a device with matching VID and PID, or until we run out of devices to check.
				//However, just in case some unexpected error occurs, keep track of the number of loops executed.
				//If the number of loops exceeds a very large number, exit anyway, to prevent inadvertent infinite looping.
				LoopCounter++;
				if(LoopCounter == 10000000) { //Surely there aren't more than 10 million devices attached to any forseeable PC...
					return FALSE;
				}
			}
		}


		//This is a callback function that gets called when a Windows message is received by the form.
		//We will receive various different types of messages, but the ones we really want to use are the WM_DEVICECHANGE messages.
		protected: virtual void WndProc( Message% m ) override {
			if(m.Msg == WM_DEVICECHANGE) {
				if(((int)m.WParam == DBT_DEVICEARRIVAL) || ((int)m.WParam == DBT_DEVICEREMOVEPENDING) || ((int)m.WParam == DBT_DEVICEREMOVECOMPLETE) || ((int)m.WParam == DBT_CONFIGCHANGED) )
				{
					//WM_DEVICECHANGE messages by themselves are quite generic, and can be caused by a number of different
					//sources, not just your USB hardware device.  Therefore, must check to find out if any changes relavant
					//to your device (with known VID/PID) took place before doing any kind of opening or closing of handles/endpoints.
					//(the message could have been totally unrelated to your application/USB device)

					if(CheckIfPresentAndGetUSBDevicePath()) { //Check and make sure at least one device with matching VID/PID is attached
						//If executes to here, this means the device is currently attached and was found.
						//This code needs to decide however what to do, based on whether or not the device was previously known to be
						//attached or not.
						if((AttachedState == FALSE) || (AttachedButBroken == TRUE)) { //Check the previous attachment state
							DWORD ErrorStatusWrite;
							DWORD ErrorStatusRead;

							//We obtained the proper device path (from CheckIfPresentAndGetUSBDevicePath() function call), and it
							//is now possible to open read and write handles to the device.
							WriteHandleToUSBDevice = CreateFile(DetailedInterfaceDataStructure->DevicePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
							ErrorStatusWrite = GetLastError();
							ReadHandleToUSBDevice = CreateFile(DetailedInterfaceDataStructure->DevicePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
							ErrorStatusRead = GetLastError();

							if((ErrorStatusWrite == ERROR_SUCCESS) && (ErrorStatusRead == ERROR_SUCCESS)) {
								AttachedState = TRUE;
								AttachedButBroken = FALSE;
							}
							else { //For some reason one or both of the read/write pipes couldn't be opened, even though the device was physically attached.  This could 
								   //happen if some other application already opened handles to the device, but did not allow read or write sharing.
								AttachedState = FALSE;		//Let the rest of this application know not to try reading/writing to the USB device
								AttachedButBroken = TRUE;	//Let this WM_DEVICECHANGE handler know to retry to open handles the next time a WM_DEVICECHANGE message occurs.
								if(ErrorStatusWrite == ERROR_SUCCESS)
									CloseHandle(WriteHandleToUSBDevice);
								if(ErrorStatusRead == ERROR_SUCCESS)
									CloseHandle(ReadHandleToUSBDevice);
							}
						}
						//else we did find the device, but AttachedState was already TRUE.  In this case, don't do anything to the read/write handles,
						//since the WM_DEVICECHANGE message presumably wasn't caused by our USB device.  
					}
					else { //Device must not be connected (or not programmed with correct firmware)
						if(AttachedState == TRUE) {		//If it is currently set to TRUE, that means the device was just now disconnected
							AttachedState = FALSE;
							CloseHandle(WriteHandleToUSBDevice);
							CloseHandle(ReadHandleToUSBDevice);
						}
						AttachedState = FALSE;
						AttachedButBroken = FALSE;
					}				 
				 }
			 }

			 Form::WndProc( m );
		}





		private: System::Void FormUpdateTimer_Tick(System::Object^  sender, System::EventArgs^  e) {
			//This timer tick event handler function is used to update the user interface on the form, based on data
			//obtained asynchronously by the ReadWriteThread and the WM_DEVICECHANGE event handler functions.

			//Check if user interface on the form should be enabled or not, based on the attachment state of the USB device.
			if(AttachedState == TRUE) {
				//Device is connected and ready to communicate, enable user interface on the form 
				StatusBox_txtbx->Text = "Device Found: AttachedState = TRUE";
			}
		
			if((AttachedState == FALSE) || (AttachedButBroken == TRUE)) {
				//Device not available to communicate. Disable user interface on the form.
				StatusBox_txtbx->Text = "Device Not Detected: Verify Connection/Correct Firmware";

				ADCValueL = 0;
				ADCValueR = 0;
				ADCValueSliderTop = 0;
				ADCValueSliderBottom = 0;

				maxADCValueL = 0;
				maxADCValueR = 0;
				maxADCValueSliderTop = 0;
				maxADCValueSliderBottom = 0;

				progressBarL->Value = 0;
				progressBarR->Value = 0;
				progressBarSliderTop->Value = 0;
				progressBarSliderBottom->Value = 0;

				numericUpDown1->Value = 0;
				numericUpDown2->Value = 0;
				numericUpDown3->Value = 0;
				numericUpDown4->Value = 0;

				numericUpDown5->Value = 0;
				numericUpDown6->Value = 0;
				numericUpDown7->Value = 0;
				numericUpDown8->Value = 0;

				numericUpDown9->Value = 0;
			}

			//Update the various status indicators on the form with the latest info obtained from the ReadWriteThread()
			if(AttachedState == TRUE) {
				if (ADCValueL > maxADCValueL) {
					maxADCValueL = 0xFF; //ADCValueL; // Changed by Josiah Watson
				}
				if (ADCValueR > maxADCValueR) {
					maxADCValueR = 0xFF; //ADCValueR; // Changed by Josiah Watson
				}				
				if (ADCValueSliderTop > maxADCValueSliderTop) {
					maxADCValueSliderTop = ADCValueSliderTop;
				}				
				if (ADCValueSliderBottom > maxADCValueSliderBottom) {
					maxADCValueSliderBottom = ADCValueSliderBottom;
				}				

				/* Update the maximum value of the all the progress bar*/
				progressBarL->Maximum = maxADCValueL;
				progressBarR->Maximum = maxADCValueR;
				progressBarSliderTop->Maximum = maxADCValueSliderTop;
				progressBarSliderBottom->Maximum = maxADCValueSliderBottom;

				progressBarL->Value = ADCValueL;
				progressBarR->Value = ADCValueR;
				progressBarSliderTop->Value = ADCValueSliderTop;
				progressBarSliderBottom->Value = ADCValueSliderBottom;

				numericUpDown1->Value = ADCValueL;
				numericUpDown2->Value = ADCValueR;
				numericUpDown3->Value = ADCValueSliderTop;
				numericUpDown4->Value = ADCValueSliderBottom;

				numericUpDown5->Value = ACCValueX;
				numericUpDown6->Value = ACCValueY;
				numericUpDown7->Value = ACCValueZ;
				numericUpDown8->Value = ACCValueT/2 - 30;

				numericUpDown9->Value = ADCValuePot;
			}
		}






		private: System::Void ReadWriteThread_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {

			/*This thread does the actual USB read/write operations (but only when AttachedState == TRUE) to the USB device.
			It is generally preferrable to write applications so that read and write operations are handled in a separate
			thread from the main form.  This makes it so that the main form can remain responsive, even if the I/O operations
			take a very long time to complete.

			Since this is a separate thread, this code below executes independently from the rest of the
			code in this application.  All this thread does is read and write to the USB device.  It does not update
			the form directly with the new information it obtains (such as the ANxx/POT Voltage or pushbutton state).
			The information that this thread obtains is stored in atomic global variables.
			Form updates are handled by the FormUpdateTimer Tick event handler function.

			This application sends packets to the endpoint buffer on the USB device by using the "WriteFile()" function.
			This application receives packets from the endpoint buffer on the USB device by using the "ReadFile()" function.
			Both of these functions are documented in the MSDN library.

			All ReadFile() and WriteFile() operations in this example project are synchronous.  They are blocking function
			calls and only return when they are complete, or if they fail because of some event, such as the user unplugging
			the device.  It is possible to call these functions with "overlapped" structures, and use them as non-blocking
			asynchronous I/O function calls.  

			Note:  This code may perform differently on some machines when the USB device is plugged into the host through a
			USB 2.0 hub, as opposed to a direct connection to a root port on the PC.  In some cases the data rate may be slower
			when the device is connected through a USB 2.0 hub.  This performance difference is believed to be caused by
			the issue described in Microsoft knowledge base article 940021:
			http://support.microsoft.com/kb/940021/en-us 

			Higher effective bandwidth (up to the maximum offered by interrupt endpoints), both when connected
			directly and through a USB 2.0 hub, can generally be achieved by queuing up multiple pending read and/or
			write requests simultaneously.  This can be done when using	asynchronous I/O operations (calling ReadFile() and
			WriteFile()	with overlapped structures).  The Microchip	HID USB Bootloader application uses asynchronous I/O
			for some USB operations and the source code can be used	as an example.*/
				

			// KINECT SETUP FUNCTION

			IKinectSensor *sensor = nullptr;
			IMultiSourceFrameReader *multiSourceFrameReader = nullptr;


			//Get the default Kinect sensor
			HRESULT hr = GetDefaultKinectSensor(&sensor);

			//If the function succeeds, open the sensor
			if (SUCCEEDED(hr)) {
				hr = sensor->Open();

				if (SUCCEEDED(hr)) {
					// TODO: can't get audio information
					hr = sensor->OpenMultiSourceFrameReader(
						(FrameSourceTypes_Color | FrameSourceTypes_Body | FrameSourceTypes_Depth
							| FrameSourceTypes_BodyIndex | FrameSourceTypes_Infrared),
						&multiSourceFrameReader);
				}
			}


			if (sensor == nullptr || FAILED(hr)) {
				std::cerr << "Cannot find any sensors.\n";
				return;
			}



			while (multiSourceFrameReader != nullptr) {

				// Allocate Kinect memory resources for data

				// Multisource Frame
				IMultiSourceFrame *multiSourceFrame = nullptr;
			
				// Body Frame
				IBodyFrameReference *bodyFrameReference = nullptr;
				IBodyFrame *bodyFrame = nullptr;			
			
				// Color Frame
				IColorFrameReference *colorFrameReference = nullptr;
				IColorFrame *colorFrame = nullptr;
				IFrameDescription *colorFrameDescription = nullptr;

				// Depth Frame
				IDepthFrameReference *depthFrameReference = nullptr;
				IDepthFrame *depthFrame = nullptr;
				IFrameDescription *depthFrameDescription = nullptr;

				// Error check variables
				HRESULT hr_body;
				HRESULT hr_color;
				HRESULT hr_depth;


				if (SUCCEEDED(hr)) {

					// wait until all sources are available
					while (FAILED(hr = multiSourceFrameReader->AcquireLatestFrame(&multiSourceFrame)));



					// Get Body Frame
					hr_body = multiSourceFrame->get_BodyFrameReference(&bodyFrameReference);

					if (SUCCEEDED(hr_body)) {					
						hr_body = bodyFrameReference->AcquireFrame(&bodyFrame);					

						if (SUCCEEDED(hr_body)) {	
							IBody *bodies[BODY_COUNT] = { 0 };
							hr_body = bodyFrame->GetAndRefreshBodyData(_countof(bodies), bodies);

							if (SUCCEEDED(hr_body)) {							
								processBodies(BODY_COUNT, bodies);
							
								//After body processing is done, we're done with our bodies so release them.
								for (unsigned int bodyIndex = 0; bodyIndex < _countof(bodies); bodyIndex++) {
									safeRelease(bodies[bodyIndex]);
								}							
							}

							safeRelease(bodyFrame);
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

						safeRelease(bodyFrameReference);
					} // end get body frame

				
				





					// Get Color Frame
					hr_color = multiSourceFrame->get_ColorFrameReference(&colorFrameReference);

					if (SUCCEEDED(hr_color)) {
						hr_color = colorFrameReference->AcquireFrame(&colorFrame);

						if (SUCCEEDED(hr_color)) {						
							hr_color = colorFrame->get_FrameDescription(&colorFrameDescription);

							if (SUCCEEDED(hr_color)) {
								int width = 0;
								int height = 0;
								colorFrameDescription->get_Width(&width);
								colorFrameDescription->get_Height(&height);

								colorImage.create(height, width, CV_8UC4);
								BYTE* imgDataPtr = (BYTE*) colorImage.data;
							
								hr_color = colorFrame->CopyConvertedFrameDataToArray(width * height * 4, imgDataPtr, ColorImageFormat_Bgra);

								if (SUCCEEDED(hr_color)) {
									//imread returns the image in B G R order

									if (!colorImage.empty()) // Check for invalid input
									{
										cv::Mat tempImage = colorImage;
										cv::flip(colorImage, tempImage, 1);
										colorImage = tempImage;

										cv::Size s = colorImage.size();
										//std::cout << "size: w - " << s.width / 2 << " h - " << s.height / 2 << std::endl;
										if (cv::getWindowProperty("RGB Camera",CV_WND_PROP_AUTOSIZE) != -1) {
											cv::imshow("RGB Camera", colorImage); // Show our image inside it.
											cv::resizeWindow("RGB Camera", s.width / 2, s.height / 2);
										}
									}
								}

								safeRelease(colorFrameDescription);
							}						
						
							safeRelease(colorFrame);
						}
					
						safeRelease(colorFrameReference);
					} // end get color frame

				








					// Get Depth Frame
					hr_depth = multiSourceFrame->get_DepthFrameReference(&depthFrameReference);

					if (SUCCEEDED(hr_depth)) {
						hr_depth = depthFrameReference->AcquireFrame(&depthFrame);

						if (SUCCEEDED(hr_depth)) {
							hr_depth = depthFrame->get_FrameDescription(&depthFrameDescription);

							if (SUCCEEDED(hr_depth)) {
								int width = 0;
								int height = 0;
								UINT16 minReliableDistance = 0;
								UINT16 maxReliableDistance = 0;

								depthFrameDescription->get_Width(&width);
								depthFrameDescription->get_Height(&height);
								depthFrame->get_DepthMinReliableDistance(&minReliableDistance);
								depthFrame->get_DepthMaxReliableDistance(&maxReliableDistance);

							
								depthImage_short.create(height, width, CV_16U);
								UINT16 *imgDataPtr = (UINT16*)depthImage_short.data;
							
							
								//UINT16 *imgDataPtr = NULL;
							
								//depthImage_byte.create(height, width, CV_8U);
								//BYTE *depthImageLower = (BYTE*)depthImage_byte.data;

								hr_depth = depthFrame->CopyFrameDataToArray(width * height, imgDataPtr);

								if (SUCCEEDED(hr_depth)) {
									// map the values to the depth range
									for (int i = 0; i < width * height; i++) {
										// get the lower 8 bits
										UINT16 depth = (UINT16) imgDataPtr[i];
										if (i == width*60-100) {
											ACCValueX = (signed int) depth;
										}

										//if (depth >= minReliableDistance && depth <= maxReliableDistance) {
										imgDataPtr[i] = depth * 8;
										//depthImage_short.data[i] = depth - minReliableDistance; // (BYTE)(depth - minReliableDistance);
										//}
										//imgDataPtr[i] = depth * 5;

									
										//else {
										//	imgDataPtr[i] = 0;
										//	//depthImage_short.data[i] = 0;
										//}
									}

									if (!depthImage_short.empty()) // Check for invalid input
									{
										cv::Mat tempImage = depthImage_short;
										cv::flip(depthImage_short, tempImage, 1);
										depthImage_short = tempImage;
									
										cv::Size s = depthImage_short.size();
								
										if (cv::getWindowProperty("Depth Sensor", CV_WND_PROP_AUTOSIZE) != -1) {
											cv::imshow("Depth Sensor", depthImage_short); // Show our image inside it.
											cv::resizeWindow("Depth Sensor", s.width, s.height);
										}
									}
								}
						
								safeRelease(depthFrameDescription);
							}

							safeRelease(depthFrame);
						}

						safeRelease(depthFrameReference);
					} // end get depth frame

				
					safeRelease(multiSourceFrame);
				} // end get multisource frame
			} 

			safeRelease(multiSourceFrameReader);
		}



		private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			mTouchCalibratePending = TRUE;
			maxADCValueL = 0;
			maxADCValueR = 0;
			maxADCValueSliderTop = 0;
			maxADCValueSliderBottom = 0;
		}






		// GLOBAL VARIABLES
		unsigned char lastSpeed = 0;
		unsigned char lastDirections = 0x11;
		unsigned char lastLeftSpeed = 0;
		unsigned char lastRightSpeed = 0;


		int userBodyID = -1;
		bool runInit = true;



		// CONSTANTS
		const double RAD_2_DEG = 180 / 3.14;
		const int BACKWARD = 0;
		const int FORWARD = 1;

		const int MAX_SPEED_VAL = 255;

		const double THRESH_Z = 2.0;			// meters	// previously 2.44
		const double MAX_SPEED_DIST_Z = 3.0;	// meters	// previously 3.66				
		const double SPEED_RANGE_Z = MAX_SPEED_DIST_Z - THRESH_Z;
		const double K_Z = MAX_SPEED_VAL / SPEED_RANGE_Z;		// steps per meter

		const double THRESH_ANGLE = 0.1;//testing - was 0.03;		// 0.087;	// tan(5-deg)				
		const double MAX_SPEED_ANGLE = 0.3;		//0.577;	// tan(30-deg)    was 25 deg 0.466
		const double SPEED_RANGE_ANGLE = MAX_SPEED_ANGLE; // should we subtract threshold angle?
		const double K_X = MAX_SPEED_VAL / SPEED_RANGE_ANGLE;	// steps per angle

		const int MAX_SPEED_INCREMENT = 1;

		const int MIN_SPEED_VAL_L = 10;    // previously 6
		const int MIN_SPEED_VAL_R = 15;   // previously 10
		//const double LINEAR_FIT_SLOPE_L = (double)(MAX_SPEED_VAL - MIN_SPEED_VAL_L) / MAX_SPEED_VAL;
		const double LINEAR_FIT_SLOPE_R = (double)(MAX_SPEED_VAL - MIN_SPEED_VAL_R) / MAX_SPEED_VAL; // weaker motor




		private: System::Void processBodies(const unsigned int &bodyCount, IBody **bodies) {	

			for (unsigned int bodyIndex = 0; bodyIndex < bodyCount; bodyIndex++) {
				IBody *body = bodies[bodyIndex];

				// CHECK IF TRACKING
				BOOLEAN isTracked = false;
				HRESULT hr = body->get_IsTracked(&isTracked);



				// LOST TRACKING ON THE USER
				if (isTracked == false && bodyIndex == userBodyID) {
			
					// reset initialization
					userBodyID = -1; 
					runInit = true;
			
					updateGUI(0, 0, 0);

					if (AttachedState == TRUE) { //Do not try to use the read/write handles unless the USB device is attached and ready			
				
						/* // send STOP command
							lastSpeed = 0;				
						/*/ // go forward and use the last speed					
							lastSpeed = lastSpeed;
						//*/

						// TRANSMIT SPEEDS
						unsigned char directions = (unsigned char)((FORWARD << 4) | (FORWARD)); // set left/right direction
						unsigned char leftSpeed = (unsigned char) lastSpeed; // set left speed			
						unsigned char rightSpeed = (unsigned char) lastSpeed; // set right speed
				
						// Save values for later				
						lastDirections = directions;
						lastLeftSpeed = leftSpeed;
						lastRightSpeed = rightSpeed;

						transmitData(directions, leftSpeed, rightSpeed);
					}
				}

		
				// NOT TRACKING

				if (FAILED(hr) || isTracked == false) {
					continue;
				}





				// TRACKING

				// Get joints
				Joint joints[JointType_Count];
				hr = body->GetJoints(_countof(joints), joints);
		
				if (SUCCEEDED(hr)) {
					const CameraSpacePoint &spineMidPos = joints[JointType_SpineMid].Position;

					// INITIALIZATION SEQUENCE			
					if (runInit) {				
						userBodyID = bodyIndex;
						runInit = false;
					}
			

					// CONTROL ALGORITHM
					if (bodyIndex == userBodyID) {				
				
						double posX = spineMidPos.X;
						double posY = spineMidPos.Y;
						double posZ = spineMidPos.Z;
						double angle = posX / posZ;


						// calculate forward speed
						double errZ = posZ - THRESH_Z;

						int speed;
						if (errZ <= 0) {
							speed = 0;
						}
						else if (errZ >= SPEED_RANGE_Z) {
							speed = 255;
						}
						else {
							speed = floor(K_Z * errZ);
						}

						// calculate turn speed
						double abs_angle = abs(angle);

						int turn;
						if (abs_angle <= THRESH_ANGLE) {
							turn = 0;
						}
						else {
							double diff = (angle / abs_angle) * (abs_angle - THRESH_ANGLE);
							turn = floor(K_X * diff * 0.5);
						}


						// determine motor speeds
						int LSpd = speed - turn;
						int RSpd = speed + turn;


						// check if one motor is maxxed 
						if (LSpd > MAX_SPEED_VAL) {
							RSpd -= (LSpd - MAX_SPEED_VAL);
							LSpd = MAX_SPEED_VAL;
						}
						else if (RSpd > MAX_SPEED_VAL) {
							LSpd -= (RSpd - MAX_SPEED_VAL);
							RSpd = MAX_SPEED_VAL;
						}


						// determine motor direction
						int LDir = FORWARD;
						int RDir = FORWARD;
						if (LSpd < 0) {
							LDir = BACKWARD;
						}
						if (RSpd < 0) {
							RDir = BACKWARD;
						}
			
						LSpd = abs(LSpd);
						RSpd = abs(RSpd);



						// slowly increment / decrement speed
						int nextsuLSpd = lastLeftSpeed;
						if (lastLeftSpeed < MAX_SPEED_VAL - MAX_SPEED_INCREMENT) {
							nextsuLSpd += MAX_SPEED_INCREMENT;
						}

						int nextsuRSpd = lastRightSpeed;
						if (lastRightSpeed < MAX_SPEED_VAL - MAX_SPEED_INCREMENT) {
							nextsuRSpd += MAX_SPEED_INCREMENT;
						}

						int nextsdLSpd = lastLeftSpeed;
						if (lastLeftSpeed > MAX_SPEED_INCREMENT) {
							nextsdLSpd -= MAX_SPEED_INCREMENT;
						}

						int nextsdRSpd = lastRightSpeed;
						if (lastRightSpeed > MAX_SPEED_INCREMENT) {
							nextsdRSpd -= MAX_SPEED_INCREMENT;
						}


						if (LSpd > lastLeftSpeed) {
							LSpd = nextsuLSpd;
						}
						/*else if (LSpd < lastLeftSpeed) {
							LSpd = nextsdLSpd;
						}*/

						if (RSpd > lastRightSpeed) {
							RSpd = nextsuRSpd;
						}
						/*else if (RSpd < lastRightSpeed) {
							RSpd = nextsdRSpd;
						}*/


						// fit calculated speed to linear speed function				
						if (LSpd > 0) {
							LSpd = ceil(LSpd * LINEAR_FIT_SLOPE_R) + MIN_SPEED_VAL_L; //testing
						}
						else {
							LSpd = 0;
						}
						if (RSpd > 0) {
							RSpd = ceil(RSpd * LINEAR_FIT_SLOPE_R) + MIN_SPEED_VAL_R;
						}
						else {
							RSpd = 0;
						}


						// Update GUI
						updateGUI((posZ * 100), (posX * 100), (atan(angle) * RAD_2_DEG));												


						// Transmit new speeds
						if (AttachedState == TRUE) { //Do not try to use the read/write handles unless the USB device is attached and ready				
					
							unsigned char directions = (unsigned char) ((LDir << 4) | (RDir)); // set left/right direction
							unsigned char leftSpeed = (unsigned char) LSpd; // set left speed			
							unsigned char rightSpeed = (unsigned char) RSpd; // set right speed

							// Save values for later
							lastSpeed = speed;
							lastDirections = directions;
							lastLeftSpeed = leftSpeed;
							lastRightSpeed = rightSpeed;

							transmitData(directions, leftSpeed, rightSpeed);
						}
					}
				}
			}
		}



		// Transmit data to microcontroller
		void transmitData(unsigned char dirs, unsigned char ls, unsigned char rs) {

			unsigned char OUTBuffer[65];	//Allocate a memory buffer equal to the OUT endpoint size + 1
			unsigned char INBuffer[65];		//Allocate a memory buffer equal to the IN endpoint size + 1
			DWORD BytesWritten = 0;
			DWORD BytesRead = 0;

			OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.

			OUTBuffer[1] = dirs;
			OUTBuffer[2] = ls;
			OUTBuffer[3] = rs;

			for (unsigned char i = 4; i < 65; i++) {
				OUTBuffer[i] = 0xFF;
			}

			if (WriteFile(WriteHandleToUSBDevice, &OUTBuffer, 65, &BytesWritten, 0)) {	//Blocking function, unless an "overlapped" structure is used
			
				INBuffer[0] = 0;
			
				if (ReadFile(ReadHandleToUSBDevice, &INBuffer, 65, &BytesRead, 0)) {	//Blocking function, unless an "overlapped" structure is used	
					if (INBuffer[1] == 0x20) {
						ADCValueL = ((0x00 << 8) + INBuffer[3]);	//Need to reformat the data from two unsigned chars into one unsigned int.
						ADCValueR = ((0x00 << 8) + INBuffer[4]);	//Need to reformat the data from two unsigned chars into one unsigned int.
						ADCValueSliderTop = ((0x00 << 8) + (INBuffer[2] / 16));	//Need to reformat the data from two unsigned chars into one unsigned int.
						ADCValueSliderBottom = ((0x00 << 8) + (INBuffer[2] % 16));	//Need to reformat the data from two unsigned chars into one unsigned int.
						ADCValuePot = INBuffer[5];
					}
				}
			}
		}


		// Update the GUI for debugging
		void updateGUI(int dist, int pos, int angle) {
			ACCValueX = dist;
			ACCValueY = pos;
			ACCValueZ = angle;
		}



	};

}

















// GETTING BODY FEATURES





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













