/*================================================
Author: Kevin Liu (CT8)
Date:	15 June 2023
Description: Simple class that uses Windows.h and
write file to send hex data to a serial port.
Hex data corresponds to commands for Pelco-D,
PTZ positioners.
==================================================*/

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include "nmea0183utils.h"

boolean checkTagId(std::string p_sen, std::string tagId)
{
	boolean isTag;
	size_t dollarDelim = p_sen.find('$');
	std::string recTagId = p_sen.substr(dollarDelim + 3, 3);

	if (tagId == recTagId)
	{
		isTag = true;
	}
	else
	{
		isTag = false;
	}

	return isTag;
}

const int BUFFER_SIZE = 256;

int main()
{
	// Open serial port
	HANDLE serialHandle;

	serialHandle = CreateFile("\\\\.\\COM20",
							  GENERIC_READ | GENERIC_WRITE,
							  0,
							  0,
							  OPEN_EXISTING,
							  FILE_ATTRIBUTE_NORMAL,
							  0);

	// Try open serial port
	if (serialHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "Open serial port: FAILED \n";
	}
	else
	{
		std::cout << "============================ \n";
		std::cout << "Open serial port: SUCCESS \n";
		std::cout << "============================\n";
	}

	// Data Center Bridging settings
	DCB serialParams = {0};

	GetCommState(serialHandle, &serialParams);
	serialParams.BaudRate = 38400;
	serialParams.ByteSize = 8;
	serialParams.StopBits = ONESTOPBIT;
	serialParams.Parity = NOPARITY;
	SetCommState(serialHandle, &serialParams);

	// Set timeouts
	/*COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 50;
	timeout.WriteTotalTimeoutConstant = 50;
	timeout.WriteTotalTimeoutMultiplier = 10;*/

	DWORD bytesRead = 0;
	char buffer[BUFFER_SIZE];
	std::string data;
	std::string sentence;

	while (serialHandle != INVALID_HANDLE_VALUE)
	{
		// Read data from the serial port into the buffer
		if (ReadFile(serialHandle, buffer, BUFFER_SIZE - 1, &bytesRead, NULL))
		{
			buffer[bytesRead] = '\0'; // Null-terminate the received data
			data += buffer;

			size_t pos;
			while ((pos = data.find('\n')) != std::string::npos)
			{
				// Extract and print the sentence terminated by the newline character
				sentence = data.substr(0, pos + 1);

				// Check the checksum is correct
				if (compareChecksum(sentence))
				{
					if (checkTagId(sentence, "HDG"))
					{
						sentence[5] = 'T';
						std::string newChecksum = getChecksum(sentence);
						pos = sentence.find('*');
						sentence[pos + 1] = newChecksum[0];
						sentence[pos + 2] = newChecksum[1];
						std::cout << "HDG Flagged\n";
					}
					const char *charArrSen = sentence.c_str();
					WriteFile(serialHandle,	   // Handle to the Serial port
							  charArrSen,	   // Data to be written to the port
							  sentence.size(), // No of bytes to write
							  NULL,			   // Bytes written
							  NULL);
					std::cout << "piped out: " << sentence;
				}
				// Remove the processed sentence (including the newline character) from the data string
				data.erase(0, pos + 1);
			}
		}
		else
		{
			std::cout << "Error reading from the serial port." << std::endl;
			CloseHandle(serialHandle);
			return 1;
		}
	}

	CloseHandle(serialHandle);
	system("pause");
	return 0;
}
