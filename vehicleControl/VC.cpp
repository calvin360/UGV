#include "VC.h"


using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

int VC::connect(String^ hostName, int portNumber)
{
	Console::WriteLine("Connecting");
	VCClient = gcnew TcpClient(hostName, portNumber);
	VCClient->NoDelay = true;
	VCClient->ReceiveTimeout = 500;//ms
	VCClient->SendTimeout = 500;//ms
	VCClient->ReceiveBufferSize = 1024;
	VCClient->SendBufferSize = 1024;
	String^ Str = gcnew String("5260528\n");
	SendData2 = gcnew array<unsigned char>(16);
	//ReadData = gcnew array<unsigned char>(2500);
	NetworkStream^ VCStream = VCClient->GetStream();
	SendData2 = System::Text::Encoding::ASCII->GetBytes(Str);
	VCStream->Write(SendData2, 0, SendData2->Length);
	flag = 0;
	return 1;
}
int VC::setupSharedMemory()
{
	//SM for timestamps
	tObj = new SMObject(_TEXT("timeStamps"), sizeof(timeStamps));//declaring SM
	tObj->SMAccess();
	//SM for PM
	PMObj = new SMObject(_TEXT("ProcessManagement"), sizeof(ProcessManagement));
	PMObj->SMAccess();
	//SM for VC
	VCObj = new SMObject(_TEXT("SM_VehicleControl"), sizeof(SM_VehicleControl));
	VCObj->SMAccess();
	return 1;
}
int VC::getData()
{
	SM_VehicleControl* VCPtr = (SM_VehicleControl*)VCObj->pData;
	String^ Control = gcnew String("#" + VCPtr->Steering.ToString("f2") + " " + VCPtr->Speed.ToString("f2") + flag + "#");
	Console::WriteLine(Control);
	SendData2 = System::Text::Encoding::ASCII->GetBytes(Control);
	return 1;
}
int VC::checkData()
{
	SM_VehicleControl* VCPtr = (SM_VehicleControl*)VCObj->pData;
	double speed = VCPtr->Speed;
	if ((-1<=VCPtr->Speed<=1)&&(-40<=VCPtr->Steering<=40)){
		Console::WriteLine("Good data");
		flag = !flag;
		Sleep(100);
		return 1;
	}
	else {
		Console::WriteLine("Bad data");
		return 0;
	}
}
int VC::sendDataToSharedMemory()
{
	NetworkStream^ VCStream = VCClient->GetStream();
	VCStream->Write(SendData2, 0, SendData2->Length);
	return 1;
}
bool VC::getShutdownFlag()
{
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj->pData;
	timeStamps* timePtr = (timeStamps*)tObj->pData;
	if (PMSMPtr->Shutdown.Status == 0xFF || PMSMPtr->Shutdown.Status == 0x08)
		return 1;
	if ((timePtr->VehicleControl - timePtr->PM) > (PMSMPtr->LifeCounter)) {
		Console::WriteLine("PM died");
		return 1;
	}
	return 0;
}
int VC::setHeartbeat()
{
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj->pData;
	timeStamps* timePtr = (timeStamps*)tObj->pData;
	if (PMSMPtr->Heartbeat.Flags.VehicleControl == 0)
		PMSMPtr->Heartbeat.Flags.VehicleControl = 1;
	timePtr->VehicleControl = (double)Stopwatch::GetTimestamp() / (double)Stopwatch::Frequency;
	Sleep(50);
	Console::WriteLine("VC time stamp    : {0,12:F3} {1,12:X8}", timePtr->VehicleControl, PMSMPtr->Shutdown.Status);
	return 1;
}
VC::~VC()
{
	delete tObj;
	delete PMObj;
	delete VCObj;
	Stream->Close();
	VCClient->Close();
}
