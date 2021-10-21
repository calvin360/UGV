#include "VC.h"

#define PI 3.14159265

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

int VC::connect(String^ hostName, int portNumber)
{
	//String^ AskScan = gcnew String("sRN LMDscandata");
	Client = gcnew TcpClient(hostName, portNumber);
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;
	String^ Str = gcnew String("5260528\n");
	SendData = gcnew array<unsigned char>(16);
	//ReadData = gcnew array<unsigned char>(2500);
	NetworkStream^ Stream = Client->GetStream();
	SendData = System::Text::Encoding::ASCII->GetBytes(Str);
	Stream->Write(SendData, 0, SendData->Length);
	System::Threading::Thread::Sleep(10);
	//SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);
	// Read the incoming data
	//Stream->Read(ReadData, 0, ReadData->Length);
	// Convert incoming data from an array of unsigned char bytes to an ASCII string
	//ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	// Print the received string on the screen
	//Console::WriteLine(ResponseData);
	return 1;
}
int VC::setupSharedMemory()
{
	//SM for timestamps
	tObj = new SMObject(_TEXT("timeStamps"), sizeof(timeStamps));//declaring SM
	//tObj->SMCreate();
	tObj->SMAccess();
	//timeStamps* timePtr = (timeStamps*)tObj->pData;
	//SM for PM
	PMObj = new SMObject(_TEXT("ProcessManagement"), sizeof(ProcessManagement));
	//PMObj->SMCreate();
	PMObj->SMAccess();
	//ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj->pData;
	VCObj = new SMObject(_TEXT("SM_VehicleControl"), sizeof(SM_VehicleControl));
	//LsObj->SMCreate();
	VCObj->SMAccess();
	return 1;
}
int VC::getData()
{
	SM_VehicleControl* VCPtr = (SM_VehicleControl*)LsObj->pData;
	// Write command asking for data
	NetworkStream^ Stream = Client->GetStream();
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
	StringArray = ResponseData->Split(' ');
	StartAngle = System::Convert::ToInt32(StringArray[23], 16);
	Res = System::Convert::ToInt32(StringArray[24], 16) / 10000.0;
	//VCPtr->num = System::Convert::ToInt32(StringArray[25], 16);
	return 1;
}
int VC::checkData()
{
	if (StringArray[1] == "LMDscandata") {
		Console::WriteLine("Good data");
		return 1;
	}
	else {
		Console::WriteLine("Bad data");
		return 0;
	}
}
int VC::sendDataToSharedMemory()
{
	SM_Laser* LsPtr = (SM_Laser*)LsObj->pData;
	array<double>^ Range = gcnew array<double>(LsPtr->num);
	array<double>^ RangeX = gcnew array<double>(LsPtr->num);
	array<double>^ RangeY = gcnew array<double>(LsPtr->num);
	for (int i = 0; i < LsPtr->num; i++) {
		Range[i] = System::Convert::ToInt32(StringArray[26 + i], 16);
		LsPtr->x[i] = Range[i] * sin(i * Res * PI / 180);
		LsPtr->y[i] = Range[i] * cos(i * Res * PI / 180);
		Console::WriteLine("range: {0, 12:F3} {1, 12:F3} {2, 12:F3}", i + 1, LsPtr->x[i], LsPtr->y[i]);
	}
	return 1;
}
bool VC::getShutdownFlag()
{
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj->pData;
	timeStamps* timePtr = (timeStamps*)tObj->pData;
	if (PMSMPtr->Shutdown.Status == 0xFF || PMSMPtr->Shutdown.Status == 0x01)
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
	delete LsObj;
	Stream->Close();
	Client->Close();
}


//unsigned long CRC32Value(int i)
//{
//	int j;
//	unsigned long ulCRC;
//	ulCRC = i;
//	for (j = 8; j > 0; j--)
//	{
//		if (ulCRC & 1)
//			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
//		else
//			ulCRC >>= 1;
//	}
//	return ulCRC;
//}
//
//unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */
//	unsigned char* ucBuffer) /* Data block */
//{
//	unsigned long ulTemp1;
//	unsigned long ulTemp2;
//	unsigned long ulCRC = 0;
//	while (ulCount-- != 0)
//	{
//		ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
//		ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xff);
//		ulCRC = ulTemp1 ^ ulTemp2;
//	}
//	return(ulCRC);
//}