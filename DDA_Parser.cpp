#include "DDA_Parser.h"

using namespace std;

#pragma optimize("",off)

unsigned int ReadHexUInt(char* buffer, unsigned int size)
{
	std::string strBuf;
	for (const char* it = buffer; *it != 0; *it++)
	{
		if (!isspace(*it) && !isblank(*it) && *it != '\0')
		{
			strBuf.push_back(*it);
		}
	}

	const string& cBuf = static_cast<string&>(strBuf);

	if (cBuf.size() > 0)
		return std::stoul(cBuf);
	else
		return 0;
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
				unsigned int  valueResult = ReadHexUInt(buffer, bytesToRead);
				
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
