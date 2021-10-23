#pragma once

#ifndef SMSTRUCTS_H
#define SMSTRUCTS_H

#using <System.dll>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;


#define STANDARD_LASER_LENGTH 361

struct timeStamps {
	double Camera;
	double Display;
	double Laser;
	double PM;
	double VehicleControl;
	double GPS;
};

struct SM_Laser
{
	double x[STANDARD_LASER_LENGTH];
	double y[STANDARD_LASER_LENGTH];
	int num;
};

struct SM_VehicleControl
{
	double Speed;
	double Steering;
};

struct SM_GPSData
{
	double northing;
	double easting;
	double height;
};

struct UnitFlags
{
	unsigned char	Laser : 1,				//CRITICAL
					Display : 1,			//CRITICAL
					Camera : 1,				//CRITICAL
					VehicleControl : 1,		//NONCRITICAL
					GPS : 1,				//NONCRITICAL
					Garbage : 3;			//filler padding
};

union ExecFlags
{
	UnitFlags Flags;
	unsigned char Status;
};

struct ProcessManagement
{
	ExecFlags Heartbeat;
	ExecFlags Shutdown;
	double LifeCounter;
};

#define CRITICALMASK 0x07		//0100 1111//0011 0011 //assuming display is critical
#define NONCRITICALMASK ~CRITICALMASK	//0011 0000 //assuming vehicle control is noncritical
#endif
