#include "GPS.h"


int GPS::connect(String^ hostName, int portNumber)
{
	// Pointer to TcpClent type object on managed heap
	TcpClient^ Client;
	// arrays of unsigned chars to send and receive data
	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
	String^ AskScan = gcnew String("sRN LMDscandata");
	String^ ResponseData;
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
int GPS::setupSharedMemory()
{
	//SM for timestamps
	SMObject tObj(_TEXT("timeStamps"), sizeof(timeStamps));//declaring SM
	tObj.SMCreate();
	tObj.SMAccess();
	timeStamps* timePtr = (timeStamps*)tObj.pData;
	//SM for PM
	SMObject PMObj(_TEXT("ProcessManagement"), sizeof(ProcessManagement));
	PMObj.SMCreate();
	PMObj.SMAccess();
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj.pData;
	SMObject LsObj(_TEXT("SM_Laser"), sizeof(SM_Laser));
	LsObj.SMCreate();
	LsObj.SMAccess();
	SM_Laser* LsPtr = (SM_Laser*)LsObj.pData;

	return 1;
}
int GPS::getData(array<unsigned char>^ SendData, String^ ResponseData)
{
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
	//Console::WriteLine(ResponseData);
	array<String^>^ StringArray = ResponseData->Split(' ');
	double StartAngle = System::Convert::ToInt32(StringArray[23], 16);
	double Res = System::Convert::ToInt32(StringArray[24], 16) / 10000.0;
	int NumRanges = System::Convert::ToInt32(StringArray[25], 16);
	//Console::WriteLine(StartAngle);
	//Console::WriteLine(Res);
	//Console::WriteLine(NumRanges);
	
	return 1;
}
int GPS::checkData()
{
	// YOUR CODE HERE
	return 1;
}
int GPS::sendDataToSharedMemory(int NumRanges, array<String^>^ StringArray, SM_Laser* LsPtr, double Res)
{
	array<double>^ Range = gcnew array<double>(NumRanges);
	array<double>^ RangeX = gcnew array<double>(NumRanges);
	array<double>^ RangeY = gcnew array<double>(NumRanges);

	for (int i = 0; i < NumRanges; i++) {
		Range[i] = System::Convert::ToInt32(StringArray[26 + i], 16);
		LsPtr->x[i] = Range[i] * sin(i * Res);
		LsPtr->y[i] = Range[i] * sin(i * Res);
	}
	return 1;
}
bool GPS::getShutdownFlag()
{
	// YOUR CODE HERE
	return 1;
}
int GPS::setHeartbeat(bool heartbeat)
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