/*
MotorDriver XYZ C++ Driver for PC remote control
William Knoff, May 2023, @ UR
This script is used to control 3-axes motor controller

Manual: ICAD-Series-Reference-Manual-RS232
host-controlled-operation: http://www.oesincorp.com/ReferenceManual/host-controlled-operation.htm
Command Reference Manual: http://www.oesincorp.com/ReferenceManual/software-reference-manual.htm

Acceleration Vmax Combination
Position: +/- 2147483648 Steps
Velocity: 200 - 200 KHz Step Rate
Acceleration: 40,000 - 40 Million Steps /sec2
*/

#include <iostream>
#include <stdlib.h>
#include "MotorControlXYZ.h"

MotorControlXYZ::MotorControlXYZ() : Port(L"COM5") {} //default constructor
MotorControlXYZ::MotorControlXYZ(LPCWSTR P) : Port(P) {};

// Sets up connection with serialPort
void MotorControlXYZ::serialSetup(LPCWSTR cport)
{
	HANDLE hSerial;
	hSerial = CreateFile(cport, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			std::cout << "Serial port does not exist." << std::endl;
			exit(1);
			// Serial port does not exist. Inform user.
		}
		std::cout << "Unknown error has occured: " << GetLastError() << std::endl;
		exit(1);
		// Some other error occurred. Inform user.
	}

	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
		// Error getting state
		std::cout << "Error getting comm state." << std::endl;
		exit(1);
	}
	dcbSerialParams.BaudRate = CBR_19200;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(hSerial, &dcbSerialParams)) {
		// Error setting serial port state
		std::cout << "Error setting serial port state" << std::endl;
		exit(1);
	}

	// Set timeouts
	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 10;
	timeout.WriteTotalTimeoutConstant = 50;
	timeout.WriteTotalTimeoutMultiplier = 10;
	SetCommTimeouts(hSerial, &timeout);

	// The Request-to-Send (RTS) of the serial port is used to reset the controller card. During initialization you would have to set this line to CLEAR.
	EscapeCommFunction(hSerial, CLRRTS);
	std::cout << "Serial connection successful" << std::endl;

	this->handle = hSerial;
}

// Closes the existing serial connection
void MotorControlXYZ::serialClose()
{
	CloseHandle(this->handle);
	std::cout << "Serial connection closed." << std::endl;
}

std::string MotorControlXYZ::serialRead()
{
	DWORD iSize;
	char sReceivedChar;
	std::string strReceived;

	while (true) {
		if (!ReadFile(this->handle, &sReceivedChar, 1, &iSize, 0))
		{
			std::cout << "Reading error has occured: " << GetLastError() << std::endl;
			exit(1);
			// Some other error occurred. Inform user.
		}
		if (iSize > 0) {
			strReceived += sReceivedChar;
		}
		else {
			break;
		}
	}
	std::cout << strReceived << std::endl;
}

// Writes strings to the serial port to control XYZ stage
void MotorControlXYZ::serialWrite(const std::string& strTransmit)
{
	DWORD bytesWritten;
	std::string packet = strTransmit + "\r";
	WriteFile(this->handle, packet.c_str(), packet.length(), &bytesWritten, NULL);

	// According to OES host-controlled operation guide:
	// "After sending each packet of data to the OES™ controller, sufficient time should be given to the controller to process it, usually 100msec."
	Sleep(100);

	// Reading for response
	serialRead();
}
