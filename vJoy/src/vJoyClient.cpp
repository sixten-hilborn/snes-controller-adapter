#include "stdafx.h"
#include "public.h"
#include "vjoyinterface.h"
#include "Serial.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#pragma comment( lib, "VJOYINTERFACE" )
#define  _CRT_SECURE_NO_WARNINGS

enum class Button : unsigned short
{
	b = 0x1,
	y = 0x2,
	select = 0x4,
	start = 0x8,
	up = 0x10,
	down = 0x20,
	left = 0x40,
	right = 0x80,
	a = 0x100,
	x = 0x200,
	l = 0x400,
	r = 0x800
};

bool isPressed(const unsigned short state, const Button button)
{
	return state & static_cast<unsigned short>(button);
}

unsigned char readByte(CSerial &serial)
{
	unsigned char byte;
	while (!serial.ReadData(&byte, 1))
		Sleep(1);
	return byte;
}

unsigned short readInput(CSerial &serial)
{
	const auto byte = readByte(serial);
	if (byte & 1)
	{
		_tprintf("Skipping one byte");
		return readInput(serial);
	}

	return (byte >> 1) | ((readByte(serial) >> 1) << 7);
}

int
__cdecl
_tmain(__in int argc, __in PZPWSTR argv)
{
	CSerial serial;
	if (!serial.Open(3, 9600))
	{
		_tprintf("Failed to open port!");
		return 1;
	}

	USHORT X, Y, Z, ZR, XR;							// Position of several axes
	JOYSTICK_POSITION	iReport;					// The structure that holds the full position data
	BYTE id=1;										// ID of the target vjoy device (Default is 1)
	UINT iInterface=1;								// Default target vJoy device
	BOOL ContinuousPOV=FALSE;						// Continuous POV hat (or 4-direction POV Hat)
	int count=0;


	// Get the ID of the target vJoy device
	if (argc>1 && wcslen(argv[1]))
		sscanf_s((char *)(argv[1]), "%d", &iInterface);


	// Get the driver attributes (Vendor ID, Product ID, Version Number)
	if (!vJoyEnabled())
	{
		_tprintf("vJoy driver not enabled: Failed Getting vJoy attributes.\n");
		return -2;
	}
	else
	{
		_tprintf("Vendor: %S\nProduct :%S\nVersion Number:%S\n", TEXT(GetvJoyManufacturerString()),  TEXT(GetvJoyProductString()), TEXT(GetvJoySerialNumberString()));
	};

	// Get the state of the requested device
	VjdStat status = GetVJDStatus(iInterface);
	switch (status)
	{
	case VJD_STAT_OWN:
		_tprintf("vJoy Device %d is already owned by this feeder\n", iInterface);
		break;
	case VJD_STAT_FREE:
		_tprintf("vJoy Device %d is free\n", iInterface);
		break;
	case VJD_STAT_BUSY:
		_tprintf("vJoy Device %d is already owned by another feeder\nCannot continue\n", iInterface);
		return -3;
	case VJD_STAT_MISS:
		_tprintf("vJoy Device %d is not installed or disabled\nCannot continue\n", iInterface);
		return -4;
	default:
		_tprintf("vJoy Device %d general error\nCannot continue\n", iInterface);
		return -1;
	};

	// Check which axes are supported
	BOOL AxisX  = GetVJDAxisExist(iInterface, HID_USAGE_X);
	BOOL AxisY  = GetVJDAxisExist(iInterface, HID_USAGE_Y);
	BOOL AxisZ  = GetVJDAxisExist(iInterface, HID_USAGE_Z);
	BOOL AxisRX = GetVJDAxisExist(iInterface, HID_USAGE_RX);
	BOOL AxisRZ = GetVJDAxisExist(iInterface, HID_USAGE_RZ);
	// Get the number of buttons and POV Hat switchessupported by this vJoy device
	int nButtons  = GetVJDButtonNumber(iInterface);
	int ContPovNumber = GetVJDContPovNumber(iInterface);
	int DiscPovNumber = GetVJDDiscPovNumber(iInterface);

	// Print results
	_tprintf("\nvJoy Device %d capabilities:\n", iInterface);
	_tprintf("Numner of buttons\t\t%d\n", nButtons);
	_tprintf("Numner of Continuous POVs\t%d\n", ContPovNumber);
	_tprintf("Numner of Descrete POVs\t\t%d\n", DiscPovNumber);
	_tprintf("Axis X\t\t%s\n", AxisX?"Yes":"No");
	_tprintf("Axis Y\t\t%s\n", AxisX?"Yes":"No");
	_tprintf("Axis Z\t\t%s\n", AxisX?"Yes":"No");
	_tprintf("Axis Rx\t\t%s\n", AxisRX?"Yes":"No");
	_tprintf("Axis Rz\t\t%s\n", AxisRZ?"Yes":"No");



	// Acquire the target
	if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!AcquireVJD(iInterface))))
	{
		_tprintf("Failed to acquire vJoy device number %d.\n", iInterface);
		return -1;
	}
	else
	{
		_tprintf("Acquired: vJoy device number %d.\n", iInterface);
	}

	while (true)
	{
		const auto input = readInput(serial);
		std::cout << "Input: " << readInput(serial) << std::endl;

		SetBtn(isPressed(input, Button::b), iInterface, 1);
		SetBtn(isPressed(input, Button::a), iInterface, 2);
		SetBtn(isPressed(input, Button::y), iInterface, 3);
		SetBtn(isPressed(input, Button::x), iInterface, 4);
		SetBtn(isPressed(input, Button::l), iInterface, 5);
		SetBtn(isPressed(input, Button::r), iInterface, 6);
		SetBtn(isPressed(input, Button::start), iInterface, 7);
		SetBtn(isPressed(input, Button::select), iInterface, 8);

		int x = 1;
		int y = 1;
		if (isPressed(input, Button::left))
			--x;
		if (isPressed(input, Button::right))
			++x;
		if (isPressed(input, Button::up))
			--y;
		if (isPressed(input, Button::down))
			++y;

		SetAxis(x * 16384, iInterface, HID_USAGE_X);
		SetAxis(y * 16384, iInterface, HID_USAGE_Y);
	}

	return 0;
}
