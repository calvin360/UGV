#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>

struct timeStamps {
	double GPSTimeStamp;
	double IMUTimeStamp;
	double laserTimeStamp;
	double PMTimeStamp;
};

int main(void) {
	SMObject PMObj(_TEXT("PMObj"), sizeof(timeStamps));//declaring SM
	timeStamps* timeStampsSMPtr;
	PMObj.SMAccess();
	timeStampsSMPtr = (timeStamps*)PMObj.pData;

	while (1) {
		Console::WriteLine("time = {10:F3}", timeStampsSMPtr->PMTimeStamp);
		Sleep(5000);
		if (_kbhit()) break;
	}

	return 0;
}