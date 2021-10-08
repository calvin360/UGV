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
		if (PMSMPtr->Heartbeat.Flags.VehicleControl == 0)
			PMSMPtr->Heartbeat.Flags.VehicleControl = 1;
		timePtr->VehicleControl = (double)Stopwatch::GetTimestamp() / (double)Stopwatch::Frequency;
		Console::WriteLine(timePtr->VehicleControl);
		Sleep(100);
		Console::WriteLine("VehicleControl time stamp    : {0,12:F3} {1,12:X8}", timePtr->VehicleControl, PMSMPtr->Shutdown.Status);
		if (PMSMPtr->Shutdown.Status == 0xFF || PMSMPtr->Shutdown.Status == 0x08)
			break;
		if ((timePtr->VehicleControl - timePtr->PM) > (PMSMPtr->LifeCounter)) {
			Console::WriteLine("PM died");
			break;
		}

		/*	did pm put flag down
				true->put flag up
				false->is pm time stamp older by agreed time gap*/
				//true-> shutdown all
	}
	Console::WriteLine("Vehicle Control process ended");
	Sleep(1000);
	return 0;
}