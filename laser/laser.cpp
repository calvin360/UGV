#include <Windows.h>
#include <conio.h>

#include "SMObject.h"
#include "smstructs.h"

#using <System.dll>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

int main(void) {
	SMObject tObj(_TEXT("timeStamps"), sizeof(timeStamps));//declaring SM
	tObj.SMCreate();
	tObj.SMAccess();
	timeStamps* timePtr = (timeStamps*)tObj.pData;
	SMObject PMObj(_TEXT("ProcessManagement"), sizeof(ProcessManagement));
	PMObj.SMCreate();
	PMObj.SMAccess();
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj.pData;
	//timeStampsSMPtr = (timeStamps*)PMObj.pData;

	while (!_kbhit()) {
		if (PMSMPtr->Heartbeat.Flags.Laser == 0)
			PMSMPtr->Heartbeat.Flags.Laser == 1;
		timePtr->Laser = (double)Stopwatch::GetTimestamp() / (double)Stopwatch::Frequency;
		Console::WriteLine(timePtr->Laser);
		Sleep(100);
		Console::WriteLine("Laser time stamp    : {0,12:F3} {1,12:X8}", timePtr->Laser, PMSMPtr->Shutdown.Status);
		if (PMSMPtr->Shutdown.Status==0xFF|| PMSMPtr->Shutdown.Status == 0x08)
			break;

		/*	did pm put flag down
				true->put flag up
				false->is pm time stamp older by agreed time gap*/
				//true-> shutdown all
	}
	Console::WriteLine("Laser process ended");
	Sleep(1000);
	return 0;
}