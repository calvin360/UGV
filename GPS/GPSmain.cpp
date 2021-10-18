#include <Windows.h>
#include <conio.h>

#include "SMObject.h"
#include "smstructs.h"
#include "GPS.h"

#using <System.dll>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::IO::Ports;


int main(void) {
	//SMObject tObj(_TEXT("timeStamps"), sizeof(timeStamps));
	//tObj.SMCreate();
	//tObj.SMAccess();
	//timeStamps* timePtr = (timeStamps*)tObj.pData;
	//SMObject PMObj(_TEXT("ProcessManagement"), sizeof(ProcessManagement));
	//PMObj.SMCreate();
	//PMObj.SMAccess();
	//ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj.pData;

	//SerialPort^ Port = nullptr;
	//String^ PortName = nullptr;
	//array<unsigned char>^ SendData = nullptr;
	//array<unsigned char>^ RecvData = nullptr;
	//Port = gcnew SerialPort;
	//PortName = gcnew String("COM1");
	//SendData = gcnew array<unsigned char>(16);
	//RecvData = gcnew array<unsigned char>(112);
	//Port->Open();
	//Port->Read(RecvData, 0, sizeof(SM_GPS));
	GPS myGPS;
	myGPS.setupSharedMemory();
	myGPS.connect("192.168.1.200", 24000);

	while (!_kbhit()) {
		//if (PMSMPtr->Heartbeat.Flags.GPS == 0)
		//	PMSMPtr->Heartbeat.Flags.GPS = 1;
		//timePtr->GPS = (double)Stopwatch::GetTimestamp() / (double)Stopwatch::Frequency;
		//Console::WriteLine(timePtr->GPS);
		//Sleep(100);
		//Console::WriteLine("GPS time stamp    : {0,12:F3} {1,12:X8}", timePtr->GPS, PMSMPtr->Shutdown.Status);
		//if (PMSMPtr->Shutdown.Status == 0xFF || PMSMPtr->Shutdown.Status == 0x10)
		//	break;
		//if ((timePtr->GPS - timePtr->PM) > (PMSMPtr->LifeCounter)) {
		//	Console::WriteLine("PM died");
		//	break;
		//}

	}
	Console::WriteLine("GPS process ended");
	Sleep(1000);
	return 0;
}