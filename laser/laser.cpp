#include <Windows.h>
#include <conio.h>

#include "SMObject.h"
#include "smstructs.h"

#using <System.dll>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

int main(void) {
	SMObject tObj(_TEXT("timeStamps"), sizeof(timeStamps));//declaring SM
	tObj.SMCreate();
	tObj.SMAccess();
	timeStamps* timePtr = (timeStamps*)tObj.pData;
	SMObject PMObj(_TEXT("ProcessManagement"), sizeof(ProcessManagement));
	PMObj.SMCreate();
	PMObj.SMAccess();
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj.pData;
	//timeStampsSMPtr = (timeStamps*)PMObj.pData;

	// LMS151 port number must be 23000
	int PortNumber = 23000;
	// Pointer to TcpClent type object on managed heap
	TcpClient^ Client;
	// arrays of unsigned chars to send and receive data
	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
	// String command to ask for Channel 1 analogue voltage from the PLC
	// These command are available on Galil RIO47122 command reference manual
	// available online
	String^ AskScan = gcnew String("sRN LMDscandata");
	// String to store received data for display
	String^ ResponseData;

	// Creat TcpClient object and connect to it
	Client = gcnew TcpClient("192.168.1.200", PortNumber);
	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	// unsigned char arrays of 16 bytes each are created on managed heap
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	// Convert string command to an array of unsigned char
	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);


	// Get the network streab object associated with clien so we 
	// can use it to read and write
	NetworkStream^ Stream = Client->GetStream();


	while (!_kbhit()) {
		if (PMSMPtr->Heartbeat.Flags.Laser == 0)
			PMSMPtr->Heartbeat.Flags.Laser = 1;
		timePtr->Laser = (double)Stopwatch::GetTimestamp() / (double)Stopwatch::Frequency;
		Console::WriteLine(timePtr->Laser);
		Sleep(100);
		Console::WriteLine("Laser time stamp    : {0,12:F3} {1,12:X8}", timePtr->Laser, PMSMPtr->Shutdown.Status);
		if (PMSMPtr->Shutdown.Status==0xFF|| PMSMPtr->Shutdown.Status == 0x01)
			break;
		if ((timePtr->Laser - timePtr->PM) > (PMSMPtr->LifeCounter)) {
			Console::WriteLine(timePtr->Laser - timePtr->PM);
			Console::WriteLine("PM died");
			break;
		}
		
		// Write command asking for data
		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);
		// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
		System::Threading::Thread::Sleep(10);
		// Read the incoming data
		Stream->Read(ReadData, 0, ReadData->Length);
		// Convert incoming data from an array of unsigned char bytes to an ASCII string
		ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
		// Print the received string on the screen
		Console::WriteLine(ResponseData);
	}

	Stream->Close();
	Client->Close();

	Console::ReadKey();
	Console::ReadKey();
	Console::WriteLine("Laser process ended");
	Sleep(1000);
	return 0;
}