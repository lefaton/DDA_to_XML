#include <iostream>
#include <string>
#include <vector>

#include "libs/pugixml/src/pugixml.hpp"

#define PARAMETERS_NAME_SIZE 16

struct SDDAParam{

	SDDAParam::SDDAParam()
	{

	}

	char* name;
	unsigned int bitsize;
	float interval;
};

struct SDDADefinition{

	SDDADefinition::SDDADefinition()
	{
        
	}

	unsigned int DDAversion;
	unsigned int frequency;
	float clock;
	unsigned int headerSize;
	
	std::vector<SDDAParam>inputParameters;
};

//class used for give easy access to the data structure based on the XML description file
class CDDA_FileFormat
{
public:

	CDDA_FileFormat::CDDA_FileFormat(){}
	CDDA_FileFormat::~CDDA_FileFormat(){}
	
	SDDADefinition* GetDefinition(){ return &m_definition; }
	int InitDefinition();
	void PrintDefinition();

private:
	void ReadXMLDefinitionFile();

	SDDADefinition m_definition;
};