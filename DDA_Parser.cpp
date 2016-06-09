#include "DDA_Parser.h"

using namespace std;

#pragma optimize("",off)

#define OSSwapConstInt2(x) \
    ((unsigned int)((((unsigned int)(x) & 0xff00) << 8) | \
                (((unsigned int)(x) & 0x00ff)  >> 8)))

#define OSSwapConstInt3(x) \
    ((unsigned long int)((((unsigned long int)(x) & 0xff000000) << 24) | \
                (((unsigned long int)(x) & 0x00ff0000) << 8) | \
                (((unsigned long int)(x) & 0x0000ff00) >> 8) | \
                (((unsigned long int)(x) & 0x000000ff) >> 24)));

#define OSSwapConstInt4(x) \
    ((unsigned long int)((((unsigned long int)(x) & 0xff000000) << 24) | \
                (((unsigned long int)(x) & 0x00ff0000) << 8) | \
                (((unsigned long int)(x) & 0x0000ff00) >> 8) | \
                (((unsigned long int)(x) & 0x000000ff) >> 24)));

#define SWAP_2(x) ( (((unsigned short)(x) & 0xff) << 8) | ((unsigned short)(x) >> 8) )
#define SWAP_4(x) ( ((unsigned long int)(x) << 24) | \
         (((unsigned long int)(x) << 8) & 0x00ff0000) | \
         (((unsigned long int)(x) >> 8) & 0x0000ff00) | \
         ((unsigned long int)(x) >> 24) )

unsigned int SwapHexUInt(char* buffer, unsigned int size)
{
	unsigned int valueResult = 0;

	switch (size)
	{
	case 1:
	{
		valueResult = (unsigned int)(buffer);
		break;
	}
	case 2:
	{
		valueResult = OSSwapConstInt2(buffer);
		break;
	}
	case 3:
	{
		valueResult = SWAP_2(buffer);
		break;
	}

	default:
		break;
	}
	return valueResult;
}

void CDDAParser::WriteDDAXMLFile(const char* fileName, ifstream* stream)
{	
	//XML stuff
	pugi::xml_document doc;
    


	//parsing param
	unsigned int counter = 0;
	char* buffer = new char[16];
	unsigned int freq = m_definition->frequency;
	unsigned int bytesToRead = 0;

	while (m_seekPos < m_fileSize)
	{
		std::cout << "[TEST] ";
		std::vector<SDDAParam>::iterator it = m_definition->inputParameters.begin();
		while ( it != m_definition->inputParameters.end() )
		{ 	
			if (counter == 0 || (counter % (unsigned int)(freq*it->interval) == 0))
			{
				bytesToRead = (unsigned int)it->bitsize / BYTE_SIZE;
				GetBytes(stream, buffer, bytesToRead, m_seekPos);
				
				//GetLittleEndian
				unsigned int  valueResult = SwapHexUInt(buffer, bytesToRead);			
				
				if (DEBUG_ON)
				{
					std::cout << " / " << it->name << "=" << valueResult;
				}

				m_seekPos += bytesToRead;
			}
			it++;
		}
		std::cout << std::endl;
		counter++;


		if (counter % freq == 0)
		{
			counter = 0;
		}
	}

}

unsigned int CDDAParser::GetBytes(ifstream* file, char* buffer, unsigned int size, unsigned int offset)
{
	file->seekg(offset, ios::beg);
	file->read(buffer, size);
	if (1)
	{
		std::cout << "[DEBUG]";
		for (unsigned int i = 0; i < size; i++)
		{
			std::cout << buffer[i];
		}
		std::cout << std::endl;
	}
	return size;
}

void CDDAParser::ParseFile(const char* fileName)
{
	
	m_seekPos = 0;

	ifstream myDDAfile;
	myDDAfile.open(fileName, std::ifstream::binary | ios::in);

	if (myDDAfile.is_open() && myDDAfile.good())
	{
		//get DDA version to read
		char versionChar[2];
		myDDAfile.get(*versionChar);
		unsigned int version = (unsigned char)versionChar[0];

		//actually get XML definition
		CDDA_FileFormat* myDDAFileFormat = new CDDA_FileFormat();
		if (myDDAFileFormat->InitDefinition(version) == 1)
		{
			std::cout << "DDA XML definition file loaded!\n" << std::endl;
			if (DEBUG_ON)
			{
				myDDAFileFormat->PrintDefinition();
			}
		}
		else
		{
			myDDAfile.close();
			std::cout << "[ERROR] XML definition file error" << std::endl;
			return;
		}

		m_definition = myDDAFileFormat->GetDefinition();

		// get length of file:
		myDDAfile.seekg(0, myDDAfile.end);
		m_fileSize = (unsigned int)myDDAfile.tellg();
		myDDAfile.seekg(0, myDDAfile.beg);

		//read header first
		char* headerBuffer = new char[m_definition->headerSize*sizeof(char)];
		GetBytes(&myDDAfile, headerBuffer, m_definition->headerSize, 0);

		//process header info
		unsigned int DDAVersion = 0;
		if (headerBuffer[2] == 'D' && headerBuffer[3] == 'D' && headerBuffer[4] == 'A')
		{
			m_seekPos += m_definition->headerSize;
			WriteDDAXMLFile(fileName, &myDDAfile);
		}
		else
		{
			std::cout << "[ERROR] Not a DDA file - " << fileName << std::endl;
		}

		myDDAfile.close();
		
	}
}