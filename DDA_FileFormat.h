#include <iostream>
#include <string>
#include <vector>
#include "libs/pugixml/src/pugixml.hpp"

struct SEntry{
	std::vector<char*> parameters;

};

//class used for give easy access to the data structure based on the XML description file
class CDDA_FileFormat
{
public:

	CDDA_FileFormat::CDDA_FileFormat(){}

	CDDA_FileFormat::~CDDA_FileFormat(){}

	unsigned int GetHeaderSize(){ return m_headerSize; }
	unsigned int GetReaderSize(){ return m_blocSize; }
	void OpenXMLDefinitionFile();

	void SetValues(char* infoFile);

private:

	std::string GetVal(void* seekPos);

	unsigned int m_headerSize;
	unsigned int m_blocSize; //blocSize to read for each entry in bit

};