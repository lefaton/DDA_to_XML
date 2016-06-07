/*
DDAToXML Converter - Copyright (c) 2016 Matthieu Dirrenberger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

This program idea is mainly based on the work done by Andrew Allan in 2009.
The white paper he wrote and the code he gave me was really helpfull for speedup
the reverse engineering part.
*/
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
	int InitDefinition(unsigned int version);
	void PrintDefinition();

private:
	void ReadXMLDefinitionFile(unsigned int version);

	SDDADefinition m_definition;
};