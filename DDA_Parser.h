#include "DDA_FileFormat.h"

using namespace std;

#define DEBUG_ON 1

class CDDAParser{

public:

	CDDAParser::CDDAParser() :m_seekPos(0){}
	CDDAParser::~CDDAParser(){}

	void ParseFile(const char* fileName, SDDADefinition* definition);

private:

	streampos GetBytes(ifstream* file, char* buffer, unsigned int size, unsigned int offset = 0);

	SDDADefinition* m_definition;
	unsigned int m_fileSize;
	unsigned int m_seekPos;
};