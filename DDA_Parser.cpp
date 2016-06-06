#include <iostream>
#include <fstream>

#include "DDA_Parser.h"

using namespace std;

#pragma optimize("",off)

streampos CDDAParser::GetBytes(ifstream* file, char* buffer, unsigned int size, unsigned int offset)
{
	file->seekg(offset, ios::beg);
	file->read(buffer, size);
	if (DEBUG_ON)
	{
		printf_s("[DEBUG]");
		for (unsigned int i = 0; i < size; i++)
		{
			printf_s("%c", buffer[i]);
		}
		printf_s("\n");
	}
	return file->tellg();
}

void CDDAParser::ParseFile(const char* fileName, SDDADefinition* definition)
{
	m_definition = definition;
	m_seekPos = -1;

	ifstream myDDAfile;
	myDDAfile.open(fileName, std::ifstream::binary | ios::in);

	if (myDDAfile.is_open() && myDDAfile.good())
	{
		streampos pos;

		// get length of file:
		myDDAfile.seekg(0, myDDAfile.end);
		m_fileSize = myDDAfile.tellg();
		myDDAfile.seekg(0, myDDAfile.beg);

		//read header first
		char* headerBuffer = new char[m_definition->headerSize*sizeof(char)];
		pos = GetBytes(&myDDAfile, headerBuffer, m_definition->headerSize, 0);

		//process header info
		unsigned int DDAVersion = 0;
		if (headerBuffer[2] == 'D' && headerBuffer[3] == 'D' && headerBuffer[4] == 'A')
		{
			if (headerBuffer[0] == 2)
			{
				DDAVersion = 1;//we have a Version 2 DDA file? - no traction control
				m_seekPos += m_definition->headerSize;
			}
			else if (headerBuffer[0] == 3 || headerBuffer[0] == 4)
			{
				char* DDABuffer = (char *)malloc(16*sizeof(char));
				GetBytes(&myDDAfile, DDABuffer, 16, 160); //get first 16 bytes from address 00A0 arbitrary chosen

				if (DDABuffer[14] != 255)
					DDAVersion = 2;//we have a Version 3 DDA file? - traction control
				else
					DDAVersion = 1;//we have a Version 3 DDA file? - no traction control

				m_seekPos += 296;//add 296 to the fileCursor position
			}			
		}
		else
		{
			printf_s("[ERROR] Not a DDA file - %s\n",fileName);
		}

		myDDAfile.close();
		
	}
}