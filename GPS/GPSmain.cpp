#include <SMObject.h>
#include <SMStructs.h>
#include <conio.h>


using namespace System;

int main(void) {
	//SMObject PMObj(_TEXT("PMObj"), sizeof(timeStamps));//declaring SM
	//timeStamps *timeStampsSMPtr;
	SMObject PMObj(_TEXT("ProcessManagement"), sizeof(ProcessManagement));
	PMObj.SMAccess();
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj.pData;
	//timeStampsSMPtr = (timeStamps*)PMObj.pData;

	while (1) {
		//Console::WriteLine(("{0,10:F3}"), timeStampsSMPtr->PMTimeStamp);
		Sleep(5000);
		if (PMSMPtr->Shutdown.Status) 
			break;
		if (_kbhit())
			break;

	/*	did pm put flag down
			true->put flag up
			false->is pm time stamp older by agreed time gap*/
				//true-> shutdown all
	}

	return 0;
}