
#include <UGV_module.h>
#include <smstructs.h>
#include <SMObject.h>

//unsigned long CRC32Value(int i);
//unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);

ref class Laser : public UGV_module
{

public:
	int connect(String^ hostName, int portNumber) override;
	int setupSharedMemory() override;
	int getData() override;
	int checkData() override;
	int sendDataToSharedMemory() override;
	bool getShutdownFlag() override;
	int setHeartbeat() override;
	~Laser();

protected:
	// YOUR CODE HERE (ADDITIONAL MEMBER VARIABLES THAT YOU MAY WANT TO ADD)

};