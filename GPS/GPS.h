
#include <UGV_module.h>
#include <smstructs.h>
#include <SMObject.h>

#define CRC32_POLYNOMIAL 0xEDB88320L

unsigned long CRC32Value(int i);
unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);
//#pragma pack(1)
//struct SM_GPS
//{
//	unsigned int Header;
//	unsigned char Discards1[40];
//	double northing;
//	double easting;
//	double height;
//	unsigned char Discards2[40];
//	unsigned int checkSum;
//};
////#pragma pack(pop,1)
ref class GPS : public UGV_module
{

public:
	int connect(String^ hostName, int portNumber) override;
	int setupSharedMemory() override;
	int getData() override;
	int checkData() override;
	int sendDataToSharedMemory() override;
	bool getShutdownFlag() override;
	int setHeartbeat() override;
	~GPS();

protected:
	// YOUR CODE HERE (ADDITIONAL MEMBER VARIABLES THAT YOU MAY WANT TO ADD)

};