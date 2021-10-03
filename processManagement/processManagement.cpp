
#using <System.dll>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

#include "SMStructs.h"
#include <SMObject.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

#define NUM_UNITS 5



bool IsProcessRunning(const char* processName);
void StartProcesses();
void print(ProcessManagement* PMSMPtr);

//defining start up sequence
TCHAR Units[10][20] = //
{
TEXT("LASER.exe"),
TEXT("Display.exe"),
TEXT("GPS.exe"),
TEXT("VehicleControl.exe"),
TEXT("Camera.exe")
};

int main()
{
	//setup shared memory
	SMObject tObj(_TEXT("timeStamps"), sizeof(timeStamps));//declaring SM
	tObj.SMCreate();
	tObj.SMAccess();
	timeStamps* timePtr = (timeStamps*)tObj.pData;
	/*array<String^>^ ModuleList = gcnew array<String^>{"ProcessManagement", "Laser", "Display", "Camera", "VehicleControl", "GPS" };
	array<int^> Critcal = gcnew array<int>(ModuleList->Length) { 1, 1, 1, 1, 0, 0 };
	array<Process^>^ processList = gcnew array<Process^>(ModuleList->Length);*/
	//ProcessManagement* PMSMPtr = nullptr;
	SMObject PMObj(_TEXT("ProcessManagement"), sizeof(ProcessManagement));
	PMObj.SMCreate();
	PMObj.SMAccess();
	ProcessManagement* PMSMPtr = (ProcessManagement*)PMObj.pData;
	PMSMPtr->Heartbeat.Status = 0x07; 
	PMSMPtr->LifeCounter = 3;
	timePtr->PM = (double)Stopwatch::GetTimestamp() / (double)Stopwatch::Frequency;
	//start all 5 modules
	StartProcesses();
	//Sleep(100);
	while (!_kbhit()) {
		timePtr->PM = (double)Stopwatch::GetTimestamp() / (double)Stopwatch::Frequency;
		Console::WriteLine("PM time stamp    : {0,12:F3} {1,12:X8}", timePtr->PM, PMSMPtr->Shutdown.Status);
		Console::WriteLine("Laser time stamp    : {0,12:F3} {1,12:X8}", timePtr->Laser, PMSMPtr->Shutdown.Status);
		Console::WriteLine("GPS time stamp    : {0,12:F3} {1,12:X8}", timePtr->GPS, PMSMPtr->Shutdown.Status);
		Console::WriteLine("Life    : {0,12:F3} {1,12:X8}", (timePtr->PM - timePtr->Display), PMSMPtr->Shutdown.Status);
		std::cout << PMSMPtr->LifeCounter << std::endl;
		PMSMPtr->Heartbeat.Flags.Camera = 1;
		Sleep(1000);
		//checking crit processes
		if ((CRITICALMASK & PMSMPtr->Heartbeat.Status) == CRITICALMASK) {
			PMSMPtr->Heartbeat.Flags.Laser = 0;
			PMSMPtr->Heartbeat.Flags.Display = 0;
			PMSMPtr->Heartbeat.Flags.Camera = 0;
			Console::WriteLine("crit reset");
		}
		//kill all if any crit process has timed out
		else if ((timePtr->PM - timePtr->Camera > PMSMPtr->LifeCounter) || (timePtr->PM - timePtr->Display > PMSMPtr->LifeCounter) || (timePtr->PM - timePtr->Laser > PMSMPtr->LifeCounter)){
			PMSMPtr->Shutdown.Status = 0xFF;
			Console::WriteLine("Weeder shutdown {0,8:F8}", PMSMPtr->Shutdown.Status);
			break;
		}
		//checking noncrit processes
		if ((NONCRITICALMASK & PMSMPtr->Heartbeat.Status) == NONCRITICALMASK) {
			PMSMPtr->Heartbeat.Flags.VehicleControl = 0;
			PMSMPtr->Heartbeat.Flags.GPS = 0;
		}
		else if ((timePtr->PM - timePtr->VehicleControl) > PMSMPtr->LifeCounter) {
				if (IsProcessRunning("VehicleControl") == true) {
				PMSMPtr->Shutdown.Status = 0x08;
				Console::WriteLine("Vehicle Control Killed");
			}
			StartProcesses();
			Console::WriteLine("Vehicle Control started");
		}
		else if ((timePtr->PM - timePtr->GPS) > PMSMPtr->LifeCounter) {
			if (IsProcessRunning("GPS") == true) {
				PMSMPtr->Shutdown.Status = 0x10;
				Console::WriteLine("GPS Killed");
			}
			StartProcesses();
			Console::WriteLine("GPS started");
		}

		if (PMSMPtr->Shutdown.Status == 0xFF)
			break;
	
		/*timeStampsSMPtr->PMTimeStamp = 599.034;
		Sleep(50);
		if (_kbhit()) break;*/
		//check system health
		//if ((CRITICALMASK & PMSMPtr->Heartbeat.Status) != CRITICALMASK) {
		//	//check which heartbeat is not working
		//	
			//for (int i = 1; i <= ModuleList->Length; i++) {
			//	if(i&PMSMPtr->Heartbeat.Status==1)
			//	std::cout << PMSMPtr->Heartbeat.Flags.processList[i] << std::endl;
			//}
			//for (int i = 1; i <= 32; i<<=1) {
			//	if (i & PMSMPtr->Heartbeat.Status == 1)
			//		std::cout << PMSMPtr->Heartbeat.Status << std::endl;
			//		PMSMPtr->Heartbeat.Status = PMSMPtr->Heartbeat.Status ^ 1;
			//		std::cout << PMSMPtr->Heartbeat.Status << std::endl;
			//}
		//}
		//if heartbeats are ok then reset

		//else {
		//	
		//	
		//// to kill
		//	PMSMPtr->Shutdown.Status = 0xFF;
		//}
		//if (PMSMPtr->Heartbeat.Flags.GPS == 1)
		//	PMSMPtr->Heartbeat.Flags.GPS = 0;
		//else {
		//	PMSMPtr->LifeCounter++;
		//	//may change to timestamp
		//	if (PMSMPtr->LifeCounter > 100) {
		//		if(IsProcessCritical)
		//	}
		//}
	}
	//check for heartbeat
		//iterate through all processes
			/*is the heartbeat of process[i] up
				true->put bit down
				false->increment heartbeat lost counter
					is the counter passed the limit for process[i]
						true-> is process[i] critical
							true-> shutdown all
							false-> has process[i] exits the operating system (hasexited())*/
							//false->kill();start();


	//if (PMSMPtr->Heartbeat.Status == 0x33) {
	//	std::cout << "all" << std::endl;
	//}
	//Sleep(100);
	//for (long int i = 1; i <= 16; i <<= 1) {
	//	std::cout << i << std::endl;
	//	for (int j = 0; j < 6; j++) {
	//		std::cout<<"j   " << (1 << j) << std::endl;
	//		if ((i & PMSMPtr->Heartbeat.Status) == i) {
	//			std::cout <<"   " << i << std::endl;
	//			print(PMSMPtr);
	//			PMSMPtr->Heartbeat.Status = ~(PMSMPtr->Heartbeat.Status & i);
	//			print(PMSMPtr);
	//		}
	//	//else std::cout << "zero" << std::endl;

	//	}

	Console::WriteLine(NONCRITICALMASK);
	Console::WriteLine("Process management terminated normally.");
	Sleep(1000);
	return 0;
}

void print(ProcessManagement* PMSMPtr) {
	std::cout << "start" << std::endl;
	std::cout <<"Laser " << (int)PMSMPtr->Heartbeat.Flags.Laser << std::endl;
	std::cout <<"GL " << (int)PMSMPtr->Heartbeat.Flags.Display << std::endl;
	std::cout <<"cam " << (int)PMSMPtr->Heartbeat.Flags.Camera << std::endl;
	std::cout <<"veh " << (int)PMSMPtr->Heartbeat.Flags.VehicleControl << std::endl;
	std::cout <<"gps " << (int)PMSMPtr->Heartbeat.Flags.GPS << std::endl;
}

//Is process running function
bool IsProcessRunning(const char* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp((const char *)entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}


void StartProcesses()
{
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];

	for (int i = 0; i < NUM_UNITS; i++)
	{
		if (!IsProcessRunning((const char*)Units[i]))
		{
			ZeroMemory(&s[i], sizeof(s[i]));
			s[i].cb = sizeof(s[i]);
			ZeroMemory(&p[i], sizeof(p[i]));

			if (!CreateProcess(NULL, Units[i], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &s[i], &p[i]))
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				_getch();
			}
			std::cout << "Started: " << Units[i] << std::endl;
			Sleep(100);
		}
	}
}

