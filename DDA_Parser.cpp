#include "DDA_Parser.h"

using namespace std;

#pragma optimize("",off)
#include <iostream>
#include <string.h>
#include <stdlib.h>  

using namespace std;

unsigned int swapbytes(unsigned int num, unsigned int size)
{
	unsigned int swapped32 = ((num >> 24) & 0xff) |
		((num << 8) & 0xff0000) |
		((num >> 8) & 0xff00) |
		((num << 24) & 0xff000000);

	unsigned int result = 0;

    switch(size)
    {
		case 1:
			result = num;
			break;
		case 2:
			result = num;
			break;
        case 3:
			result = swapped32 >> 16;
			break;
    }
    
	return result;
}

unsigned int ReadHexUInt(char* buffer, unsigned int size)
{
	// copy char* to int.
	unsigned int param = 0;
	memcpy(&param, buffer, size);

	return swapbytes(param, size);
}

void CDDAParser::WriteDDAXMLFile(const char* fileName, ifstream* stream)
{	
	//XML stuff
	pugi::xml_document doc;

	pugi::xml_node rootNode = doc.append_child("description");
	rootNode.set_name("DDA file");
 
	//parsing param
	unsigned long globalTiming = 0;
	unsigned int counter = 0;
	char* buffer = new char[BUFFER_SIZE];
	unsigned int freq = m_definition->frequency;
	unsigned int bytesToRead = 0;

	//keeping last value set for each param for fill empty data every 1/frequency / second
	unsigned int paramPos = 0;
	std::vector<unsigned int> lastValue;
	std::vector<SDDAParam>::iterator it = m_definition->inputParameters.begin();
	while (it != m_definition->inputParameters.end())
	{
		lastValue.push_back(0);
		it++;
	}
	
	std::cout << "Decoding DDA file:" << std::endl;
	while (m_seekPos < m_fileSize)
	{
		//add XML entry for every param at each 1/frequency /second read
		pugi::xml_node entryNode = rootNode.append_child("entry");
		entryNode.append_attribute("timing");
		entryNode.attribute("timing").set_value((float)globalTiming / freq);

		std::cout << "\r" << (m_seekPos/(m_fileSize / 100)) << "%";
		it = m_definition->inputParameters.begin();
		while ( it != m_definition->inputParameters.end() )
		{		
			for (int i = 0; i < BUFFER_SIZE; ++i)
			{
				buffer[i] = '\0' + i;
			}
			
			if (counter == 0 || (counter % (unsigned int)(freq*it->interval) == 0))
			{
				bytesToRead = (unsigned int)it->bitsize / BYTE_SIZE;
				GetBytes(stream, buffer, bytesToRead, m_seekPos);

				//GetLittleEndian
				unsigned int  valueResult = ReadHexUInt(buffer, bytesToRead);

				lastValue[paramPos] = valueResult;//save last

				if (DEBUG_ON)
				{
					std::cout << "  " << it->name << "=" << valueResult;
				}

				m_seekPos += bytesToRead;
			}

			//write XML entry parameters
			entryNode.append_attribute(it->name);
			unsigned int valToWrite = lastValue[paramPos];
			//handle specific operations
			if (it->op != SDDAParam::nil)
			{
				if (it->op == SDDAParam::add)
					valToWrite += it->val;
				if (it->op == SDDAParam::sub)
					valToWrite -= it->val;
				if (it->op == SDDAParam::mul)
					valToWrite *= it->val;
				if (it->op == SDDAParam::div)
					valToWrite /= it->val;
				if (it->op == SDDAParam::dif && valToWrite == it->val)
					valToWrite = 0;
			}

			entryNode.attribute(it->name).set_value((unsigned int)valToWrite);

			it++;
			paramPos++;
		}
		paramPos = 0;

		if (DEBUG_ON)
		{
			std::cout << std::endl;
		}
		counter++;


		if (counter % freq == 0)
		{
			counter = 0;
		}
		globalTiming++;
	}
	delete[] buffer;

	//write XML
	string fileNameXml = fileName;
	fileNameXml = fileNameXml.substr(0, fileNameXml.size() - 3) + "xml";
	std::cout << std::endl << std::endl << "Writing XML : ";
	if (doc.save_file(fileNameXml.c_str()))
	{
		std::cout << "OK";
	}
	else
	{
		std::cout << "ERROR";
	}
	std::cout << std::endl;
}

unsigned int CDDAParser::GetBytes(ifstream* file, char* buffer, unsigned int size, unsigned int offset)
{
	file->seekg(offset, ios::beg);
	file->read(buffer, size);
	
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
		delete[] headerBuffer;

		myDDAfile.close();
		
	}
}
