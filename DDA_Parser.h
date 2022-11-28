/*
DDA_Reader - Copyright (C) 2016 Matthieu Dirrenberger

This program idea is mainly based on the work done by Andrew Allan in 2009.
The white paper he wrote and the code he gave me was really helpfull for speedup
the reverse engineering part (http://www.ducati.ms/).

This software XML parser/writer is based on pugixml library (http://pugixml.org).
pugixml is Copyright (C) 2006-2015 Arseny Kapoulkine.
*/

#include "DDA_FileFormat.h"

using namespace std;

#define DEBUG_ON 0
#define BYTE_SIZE 8
#define BUFFER_SIZE 16

class CDDAParser{

public:

	CDDAParser() :m_seekPos(0), m_fileSize(0){}
	~CDDAParser(){}

	void ParseFile(const char* fileName);

private:

	unsigned int GetBytes(ifstream* file, char* buffer, unsigned int size, unsigned int offset = 0);
	void WriteDDAXMLFile(const char* fileName, ifstream* stream);

	SDDADefinition* m_definition;
	unsigned int m_fileSize;
	unsigned int m_seekPos;
};
