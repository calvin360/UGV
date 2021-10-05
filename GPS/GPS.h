
#include <UGV_module.h>
#include <smstructs.h>
#include <SMObject.h>

#define CRC32_POLYNOMIAL 0xEDB88320L

unsigned long CRC32Value(int i);
unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);

ref class GPS : public UGV_module
{

public:
	int connect(String^ hostName, int portNumber) override;
	int setupSharedMemory() override;
	int getData(array<unsigned char>^ SendData, String^ ResponseData) override;
	int checkData() override;
	int sendDataToSharedMemory(int NumRanges, array<String^>^ StringArray, SM_Laser* LsPtr, double Res) override;
	bool getShutdownFlag() override;
	int setHeartbeat(bool heartbeat) override;
	~GPS();

protected:
	// YOUR CODE HERE (ADDITIONAL MEMBER VARIABLES THAT YOU MAY WANT TO ADD)

};