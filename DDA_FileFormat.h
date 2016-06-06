#include <iostream>
#include <string>
#include <vector>

#include "libs/pugixml/src/pugixml.hpp"

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

	SDDADefinition::~SDDADefinition()
	{
		//std::vector<SDDAParam>::iterator it = inputParameters.begin();
		//while (it != inputParameters.end())
		//{
		//	free (it->name);
		//	it++;
		//}
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