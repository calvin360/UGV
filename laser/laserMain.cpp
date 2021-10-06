#include <Windows.h>
#include <conio.h>

#include <SMObject.h>
#include "smstructs.h"
#include <UGV_module.h>
#include "Laser.h"

#using <System.dll>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

int main(void) {
	Laser myLaser;
	myLaser.setupSharedMemory();
	myLaser.connect("192.168.1.200", 23000);

	while (!_kbhit()) {
		myLaser.setHeartbeat();
		if (myLaser.getShutdownFlag() == 1) {
			break;
		}
		myLaser.getData();
		myLaser.sendDataToSharedMemory();
	}

	Console::WriteLine("Laser process ended");
	Sleep(1000);
	return 0;
}
		//if (PMSMPtr->Heartbeat.Flags.Laser == 0)
	//SMObject tObj(_TEXT("timeStamps"), sizeof(timeStamps));//declaring SM
	//tObj.SMCreate();
	//tObj.SMAccess();
	//timeStamps* timePtr = (timeStamps*)tObj.pData;
	////SM for PM
	//SMObject PMObj(_TEXT("ProcessManagement"), sizeof(ProcessManagement));
	//PMObj.SMCreate();
	//PMObj.SMAccess();
	//ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj.pData;
	////SM for laser data
	//SMObject LsObj(_TEXT("SM_Laser"), sizeof(SM_Laser));
	//LsObj.SMCreate();
	//LsObj.SMAccess();
	//SM_Laser* LsPtr = (SM_Laser*)LsObj.pData;


	//// LMS151 port number must be 23000
	//int PortNumber = 23000;
	//// Pointer to TcpClent type object on managed heap
	//TcpClient^ Client;
	//// arrays of unsigned chars to send and receive data
	//array<unsigned char>^ SendData;
	//array<unsigned char>^ ReadData;
	//// String command to ask for Channel 1 analogue voltage from the PLC
	//// These command are available on Galil RIO47122 command reference manual
	//// available online
	//String^ AskScan = gcnew String("sRN LMDscandata");
	//// String to store received data for display
	//String^ ResponseData;
	//// Creat TcpClient object and connect to it
	//Client = gcnew TcpClient("192.168.1.200", PortNumber);
	//// Configure connection
	//Client->NoDelay = true;
	//Client->ReceiveTimeout = 500;//ms
	//Client->SendTimeout = 500;//ms
	//Client->ReceiveBufferSize = 1024;
	//Client->SendBufferSize = 1024;
	//String^ Str = gcnew String("5260528\n");
	//// unsigned char arrays of 16 bytes each are created on managed heap
	//SendData = gcnew array<unsigned char>(16);
	//ReadData = gcnew array<unsigned char>(2500);
	//// Get the network streab object associated with client so we 
	//// can use it to read and write
	//NetworkStream^ Stream = Client->GetStream();
	////semd zID and wait for response
	//// Convert string command to an array of unsigned char
	//SendData = System::Text::Encoding::ASCII->GetBytes(Str);
	//Stream->Write(SendData, 0, SendData->Length);
	//System::Threading::Thread::Sleep(10);
	//SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);
	//// Read the incoming data
	//Stream->Read(ReadData, 0, ReadData->Length);
	//// Convert incoming data from an array of unsigned char bytes to an ASCII string
	//ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	//// Print the received string on the screen
	//Console::WriteLine(ResponseData);
	
		//	PMSMPtr->Heartbeat.Flags.Laser = 1;
		//timePtr->Laser = (double)Stopwatch::GetTimestamp() / (double)Stopwatch::Frequency;
		//Console::WriteLine(timePtr->Laser);
		//Sleep(100);
		//Console::WriteLine("Laser time stamp    : {0,12:F3} {1,12:X8}", timePtr->Laser, PMSMPtr->Shutdown.Status);
		//if (PMSMPtr->Shutdown.Status == 0xFF || PMSMPtr->Shutdown.Status == 0x01)
		//	break;
		//if ((timePtr->Laser - timePtr->PM) > (PMSMPtr->LifeCounter)) {
		//	Console::WriteLine(timePtr->Laser - timePtr->PM);
		//	Console::WriteLine("PM died");
		//	break;
		//}

		// Write command asking for data
		//Stream->WriteByte(0x02);
		//Stream->Write(SendData, 0, SendData->Length);
		//Stream->WriteByte(0x03);
		//// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
		//System::Threading::Thread::Sleep(10);
		//// Read the incoming data
		//Stream->Read(ReadData, 0, ReadData->Length);
		//// Convert incoming data from an array of unsigned char bytes to an ASCII string
		//ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
		//// Print the received string on the screen
		////Console::WriteLine(ResponseData);
		//StringArray = ResponseData->Split(' ');
		//StartAngle = System::Convert::ToInt32(StringArray[23], 16);
		//Res= System::Convert::ToInt32(StringArray[24], 16)/10000.0;
		//NumRanges = System::Convert::ToInt32(StringArray[25], 16);
		////Console::WriteLine(StartAngle);
		////Console::WriteLine(Res);
		////Console::WriteLine(NumRanges);
		//array<double>^ Range = gcnew array<double>(NumRanges);
		//array<double>^ RangeX = gcnew array<double>(NumRanges);
		//array<double>^ RangeY = gcnew array<double>(NumRanges);

		//for (int i = 0; i < NumRanges; i++) {
		//	Range[i]= System::Convert::ToInt32(StringArray[26+i], 16);
		//	LsPtr->x[i] = Range[i] * sin(i * Res);
		//	LsPtr->y[i] = Range[i] * sin(i * Res);
		//}
		//for (int i = 0; i < NumRanges; i++) {
		//	Console::WriteLine("range: {0, 12:F3} {1, 12:F3}", LsPtr->x[i], LsPtr->y[i]);
		//}