#include "GPS.h"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::IO::Ports;

int GPS::connect(String^ hostName, int portNumber)
{
	// Pointer to TcpClent type object on managed heap
	Client1 = gcnew TcpClient(hostName, portNumber);
	// arrays of unsigned chars to send and receive data
	array<unsigned char>^ SendData1;
	array<unsigned char>^ ReadData1;
	String^ ResponseData1;
	Client1 = gcnew TcpClient(hostName, portNumber);
	Client1->NoDelay = true;
	Client1->ReceiveTimeout = 500;//ms
	Client1->SendTimeout = 500;//ms
	Client1->ReceiveBufferSize = 1024;
	Client1->SendBufferSize = 1024;
	NetworkStream^ Stream1 = Client1->GetStream();

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
	//SM for GPS
	GPSObj = new SMObject(_TEXT("SM_GPS"), sizeof(SM_GPS));
	GPSObj->SMCreate();
	GPSObj->SMAccess();
	SM_GPS* GPSPtr = (SM_GPS*)GPSObj->pData;
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
	Stream->Read(ReadData, 0, ReadData->Length);
	Header = 0;
	int i = 0;
	do {
		Data = ResponseData1[i++];
		Header = ((Header << 8) | Data);

	} while (Header != 0xaa44121c);
	Start = i - 4;
	
	if (Header == 0xaa44121c) {
		BytePtr = (unsigned char*)GPSPtr;
		for (int i = Start; i < (Start + sizeof(SM_GPS)); i++) {
			*(BytePtr++) = ResponseData1[i];
		}
	}

	return 1;
}
int GPS::checkData()
{

	return 1;
}
int GPS::sendDataToSharedMemory()
{
	//array<double>^ Range = gcnew array<double>(NumRanges);
	//array<double>^ RangeX = gcnew array<double>(NumRanges);
	//array<double>^ RangeY = gcnew array<double>(NumRanges);

	//for (int i = 0; i < NumRanges; i++) {
	//	Range[i] = System::Convert::ToInt32(StringArray[26 + i], 16);
	//	LsPtr->x[i] = Range[i] * sin(i * Res);
	//	LsPtr->y[i] = Range[i] * sin(i * Res);
	//}
	return 1;
}
bool GPS::getShutdownFlag()
{
	// YOUR CODE HERE
	return 1;
}
int GPS::setHeartbeat()
{
	// YOUR CODE HERE
	return 1;
}
GPS::~GPS()
{
	// YOUR CODE HERE
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