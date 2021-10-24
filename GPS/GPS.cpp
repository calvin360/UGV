#include "GPS.h"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::IO::Ports;

//putting GPS data in local struct for processing
#pragma pack(1)
struct GPSStruct
{
	unsigned int Header;
	unsigned char Discards1[40];
	double northing;
	double easting;
	double height;
	unsigned char Discards2[40];
	unsigned int checkSum;

};
GPSStruct NovatelGPS;
unsigned char* BytePtr;

int GPS::connect(String^ hostName, int portNumber)
{
	// Pointer to TcpClent type object on managed heap
	GPSClient = gcnew TcpClient(hostName, portNumber);
	GPSClient->NoDelay = true;
	GPSClient->ReceiveTimeout = 500;//ms
	GPSClient->SendTimeout = 500;//ms
	GPSClient->ReceiveBufferSize = 1024;
	GPSClient->SendBufferSize = 1024;

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
	//SM for GPS data
	GPSDataObj = new SMObject(_TEXT("SM_GPSData"), sizeof(SM_GPSData));
	GPSDataObj->SMCreate();
	GPSDataObj->SMAccess();
	SM_GPSData* GPSDataPtr = (SM_GPSData*)GPSDataObj->pData;
	return 1;
}
int GPS::getData()
{
	//trap header
	NetworkStream^ GPSStream = GPSClient->GetStream();
	ReadData1 = gcnew array<unsigned char>(sizeof(NovatelGPS) * 2);
	unsigned int Header = 0;
	int i = 0;
	int Start = 0;
	unsigned char Data;

	if (GPSStream->DataAvailable) {
		GPSStream->Read(ReadData1, 0, ReadData1->Length);
		do {
			Data = ReadData1[i++];
			Header = ((Header << 8) | Data);

		} while (Header != 0xaa44121c);
		Start = i - 4;
		//point to start of good data
		BytePtr = nullptr;
		if (Header == 0xaa44121c) {
			BytePtr = (unsigned char*)&NovatelGPS;
			for (int i = Start; i < Start + sizeof(NovatelGPS); i++) {
				*(BytePtr++) = ReadData1[i];
			}
		}
		Sleep(100);
	}

	return 1;
}
int GPS::checkData()
{
	unsigned long CRC = (unsigned long)NovatelGPS.checkSum;
	unsigned long calcCRC = CalculateBlockCRC32(sizeof(NovatelGPS) - sizeof(unsigned int), (unsigned char*)&NovatelGPS);
	if (CRC == calcCRC) {
		Console::Write("checksum: {0,8}\t", CRC);
		Console::WriteLine("calcCRC: {0,9}\t", calcCRC);
	}
		return 1;
}
int GPS::sendDataToSharedMemory()
{
	SM_GPSData* GPSDataPtr = (SM_GPSData*)GPSDataObj->pData;
	GPSDataPtr->northing = NovatelGPS.northing;
	GPSDataPtr->easting = NovatelGPS.easting;
	GPSDataPtr->height = NovatelGPS.height;
	Console::Write("northing: {0,8:N3}\t", GPSDataPtr->northing);
	Console::Write("easting: {0,9:N3}\t", GPSDataPtr->easting);
	Console::Write("height: {0,10:N3}\t", GPSDataPtr->height);
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
	Sleep(100);
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