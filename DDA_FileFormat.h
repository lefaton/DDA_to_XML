#include <iostream>

enum EDataType{
	int,
	unsigned int 8,
	unsigned int 16,
	unsigned int 32,
	float,
	bool,
	long,
	endOfTypeList,
};

//class used for give easy access to the data structure based on the XML description file
class CDDA_FileFormat
{
public:
	CDDA_FileFormat();
	unsigned int GetHeaderSize(){ return m_headerSize; }
	unsigned int GetReaderSize(){ return m_blocSize; }

	void SetValues(char* infoFile);

private:
	unsigned int m_headerSize;
	unsigned int m_blocSize; //blocSize to read for each entry in bit

};