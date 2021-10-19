#include "GPS.h"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::IO::Ports;

//SM_GPSData NovatelGPS;

int GPS::connect(String^ hostName, int portNumber)
{
	// Pointer to TcpClent type object on managed heap
	GPSClient = gcnew TcpClient(hostName, portNumber);
	GPSClient->NoDelay = true;
	GPSClient->ReceiveTimeout = 500;//ms
	GPSClient->SendTimeout = 500;//ms
	GPSClient->ReceiveBufferSize = 1024;
	GPSClient->SendBufferSize = 1024;
	// arrays of unsigned chars to send and receive data
	ReadData1 = gcnew array<unsigned char>(sizeof(SM_GPS));


	//SerialPort^ Port = nullptr;
	//String^ PortName = nullptr;
	//Port = gcnew SerialPort;
	////PortName = gcnew String("COM1");
	//SendData = gcnew array<unsigned char>(16);
	//RecvData = gcnew array<unsigned char>(112);
	//Port->PortName = gcnew String("COM1");
	//Port->BaudRate = 115200;
	//Port->StopBits = StopBits::One;
	//Port->DataBits = 8;
	//Port->Parity = Parity::None;
	//Port->Handshake = Handshake::None;
	//Port->ReadTimeout = 500;
	//Port->WriteTimeout = 500;
	//Port->ReadBufferSize = ;
	//Port->WriteBufferSize = 1024;
	//Port->Open();
	//Port->Read(RecvData, 0, sizeof(SM_GPS));

	return 1;
}
int GPS::setupSharedMemory()
{
	//SM for timestamps
	tObj = new SMObject(_TEXT("timeStamps"), sizeof(timeStamps));//declaring SM
	tObj->SMCreate();
	tObj->SMAccess();
	timeStamps* timePtr = (timeStamps*)tObj->pData;
	//SM for PM
	PMObj = new SMObject(_TEXT("ProcessManagement"), sizeof(ProcessManagement));
	PMObj->SMCreate();
	PMObj->SMAccess();
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj->pData;
	//SM for GPS dump
	GPSObj = new SMObject(_TEXT("SM_GPS"), sizeof(SM_GPS));
	GPSObj->SMCreate();
	GPSObj->SMAccess();
	SM_GPS* GPSPtr = (SM_GPS*)GPSObj->pData;
	//SM for GPS data
	GPSDataObj = new SMObject(_TEXT("SM_GPSData"), sizeof(SM_GPSData));
	GPSDataObj->SMCreate();
	GPSDataObj->SMAccess();
	SM_GPSData* GPSData = (SM_GPSData*)GPSDataObj->pData;
	return 1;
}
int GPS::getData()
{
	SM_GPS* GPSPtr = (SM_GPS*)GPSObj->pData;
	//// Write command asking for data
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
	//array<String^>^ StringArray = ResponseData->Split(' ');
	//double StartAngle = System::Convert::ToInt32(StringArray[23], 16);
	//double Res = System::Convert::ToInt32(StringArray[24], 16) / 10000.0;
	//int NumRanges = System::Convert::ToInt32(StringArray[25], 16);
	NetworkStream^ GPSStream = GPSClient->GetStream();
	GPSStream->Read(ReadData1, 0, ReadData1->Length);
	//trap header
	Header = 0;
	int i = 0;
	do {
		Data = ReadData1[i++];
		Header = ((Header << 8) | Data);

	} while (Header != 0xaa44121c);
	Start = i - 4;
	//store data
	if (Header == 0xaa44121c) {
		BytePtr = (unsigned char*)GPSPtr;
		for (int i = Start; i < (Start + sizeof(SM_GPS)); i++) {
			*(BytePtr++) = ReadData1[i];
			Console::WriteLine("11111111");
		}
	}
	Console::WriteLine(ReadData1);
	Console::WriteLine("easting: {0:F3}", GPSPtr->easting);

	return 1;
}
int GPS::checkData()
{
	SM_GPS* GPSPtr = (SM_GPS*)GPSObj->pData;
	// Check CRC is correct
	unsigned long CRC = (unsigned long)GPSPtr->checkSum;
	//unsigned long calcCRC = CalculateBlockCRC32(sizeof(SM_GPS) - 4, (unsigned char*)GPSPtr);

	//if (CRC == calcCRC) {
	//	// Print GPS data
	//	Console::Write("northing: {0,8:N3}\t", GPSPtr->northing);
	//	Console::Write("easting: {0,9:N3}\t", GPSPtr->easting);
	//	Console::Write("height: {0,10:N3}\t", GPSPtr->height);
	//	Console::Write("checksum: {0,8}\t", CRC);
	//	Console::WriteLine("calcCRC: {0,9}\t", calcCRC);
	return 1;
}
int GPS::sendDataToSharedMemory()
{
	//int n = GPSPtr->numData;
	//GPSPtr->northing[n] = NovatelGPS->northing;
	//GPSPtr->easting[n] = NovatelGPS->easting;
	//GPSPtr->height[n] = NovatelGPS->height;
	//GPSPtr->numData++;
	return 1;
}
bool GPS::getShutdownFlag()
{
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj->pData;
	timeStamps* timePtr = (timeStamps*)tObj->pData;
	if (PMSMPtr->Shutdown.Status == 0xFF || PMSMPtr->Shutdown.Status == 0x10)
		return 1;
	if ((timePtr->GPS - timePtr->PM) > (PMSMPtr->LifeCounter)) {
		Console::WriteLine("PM died");
		return 1;
	}
	return 0;
}
int GPS::setHeartbeat()
{
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj->pData;
	timeStamps* timePtr = (timeStamps*)tObj->pData;
	if (PMSMPtr->Heartbeat.Flags.GPS == 0)
		PMSMPtr->Heartbeat.Flags.GPS = 1;
	timePtr->GPS = (double)Stopwatch::GetTimestamp() / (double)Stopwatch::Frequency;
	Console::WriteLine(timePtr->GPS);
	Sleep(50);
	return 1;
}
GPS::~GPS()
{
	delete tObj;
	delete PMObj;
	delete GPSObj;
	delete GPSDataObj;
	Stream->Close();
	Client->Close();
}


unsigned long CRC32Value(int i)
{
	int j;
	unsigned long ulCRC;
	ulCRC = i;
	for (j = 8; j > 0; j--)
	{
		if (ulCRC & 1)
			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
		else
			ulCRC >>= 1;
	}
	return ulCRC;
}

unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */
	unsigned char* ucBuffer) /* Data block */
{
	unsigned long ulTemp1;
	unsigned long ulTemp2;
	unsigned long ulCRC = 0;
	while (ulCount-- != 0)
	{
		ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
		ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xff);
		ulCRC = ulTemp1 ^ ulTemp2;
	}
	return(ulCRC);
}