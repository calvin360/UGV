#include "Laser.h"

#define PI 3.14159265

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

int Laser::connect(String^ hostName, int portNumber)
{
	String^ AskScan = gcnew String("sRN LMDscandata");
	Client = gcnew TcpClient(hostName, portNumber);
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;
	String^ Str = gcnew String("5260528\n");
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	NetworkStream^ Stream = Client->GetStream();
	//authentication
	SendData = System::Text::Encoding::ASCII->GetBytes(Str);
	Stream->Write(SendData, 0, SendData->Length);
	System::Threading::Thread::Sleep(10);
	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);
	// Read the incoming data
	Stream->Read(ReadData, 0, ReadData->Length);
	// Convert incoming data from an array of unsigned char bytes to an ASCII string
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	// Print the received string on the screen
	Console::WriteLine(ResponseData);
	return 1;
}
int Laser::setupSharedMemory()
{
	//SM for timestamps
	tObj = new SMObject (_TEXT("timeStamps"), sizeof(timeStamps));//declaring SM
	tObj->SMCreate();
	tObj->SMAccess();
	timeStamps* timePtr = (timeStamps*)tObj->pData;
	//SM for PM
	PMObj = new SMObject (_TEXT("ProcessManagement"), sizeof(ProcessManagement));
	PMObj->SMCreate();
	PMObj->SMAccess();
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj->pData;
	LsObj = new SMObject (_TEXT("SM_Laser"), sizeof(SM_Laser));
	LsObj->SMCreate();
	LsObj->SMAccess();
	SM_Laser* LsPtr = (SM_Laser*)LsObj->pData;
	return 1;
}
int Laser::getData()
{
	SM_Laser* LsPtr = (SM_Laser*)LsObj->pData;
	// Write command asking for data
	NetworkStream^ Stream = Client->GetStream();
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	System::Threading::Thread::Sleep(20);
	// Read the incoming data
	Stream->Read(ReadData, 0, ReadData->Length);
	// Convert incoming data from an array of unsigned char bytes to an ASCII string
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	// Print the received string on the screen
	StringArray = ResponseData->Split(' ');
	StartAngle = System::Convert::ToInt32(StringArray[23], 16);
	Res = System::Convert::ToInt32(StringArray[24], 16) / 10000.0;
	LsPtr->num = System::Convert::ToInt32(StringArray[25], 16);
	return 1;
}
int Laser::checkData()
{
	SM_Laser* LsPtr = (SM_Laser*)LsObj->pData;
	if (StringArray[1] == "LMDscandata"&&LsPtr->num==361&&StringArray->Length==393) {
		Console::WriteLine("Good data");
		Sleep(100);
		return 1;
	}
	else {
		Console::WriteLine("Bad data");
		Sleep(100);
		return 0;
	}
}
int Laser::sendDataToSharedMemory()
{
	SM_Laser* LsPtr = (SM_Laser*)LsObj->pData;
	array<double>^ Range = gcnew array<double>(LsPtr->num);
	array<double>^ RangeX = gcnew array<double>(LsPtr->num);
	array<double>^ RangeY = gcnew array<double>(LsPtr->num);
	for (int i = 0; i < LsPtr->num; i++) {
		Range[i] = System::Convert::ToInt32(StringArray[26 + i], 16);
		LsPtr->x[i] = Range[i] * sin(i * Res * PI / 180);
		LsPtr->y[i] = Range[i] * cos(i * Res * PI / 180);
	}
	return 1;
}
bool Laser::getShutdownFlag()
{
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj->pData;
	timeStamps* timePtr = (timeStamps*)tObj->pData;
	if (PMSMPtr->Shutdown.Status == 0xFF || PMSMPtr->Shutdown.Status == 0x01)
		return 1;
	if ((timePtr->Laser - timePtr->PM) > (PMSMPtr->LifeCounter)) {
		Console::WriteLine("PM died");
		return 1;
	}
	return 0;
}
int Laser::setHeartbeat()
{
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj->pData;
	timeStamps* timePtr = (timeStamps*)tObj->pData;
	if (PMSMPtr->Heartbeat.Flags.Laser == 0)
		PMSMPtr->Heartbeat.Flags.Laser = 1;
	timePtr->Laser = (double)Stopwatch::GetTimestamp() / (double)Stopwatch::Frequency;
	Sleep(50);
	Console::WriteLine("Laser time stamp    : {0,12:F3} {1,12:X8}", timePtr->Laser, PMSMPtr->Shutdown.Status);
	return 1;
}
Laser::~Laser()
{
	delete tObj;
	delete PMObj;
	delete LsObj;
	Stream->Close();
	Client->Close();
}
