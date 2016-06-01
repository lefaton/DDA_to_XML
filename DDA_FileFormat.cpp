#include "DDA_FileFormat.h"

void CDDA_FileFormat::OpenXMLDefinitionFile()
{


	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file("DDA_Definition.xml");

	std::cout << "Load result: " << result.description() << "DDA Version: " << doc.child("DDA_Definition").attribute("version").value() << std::endl;

	char yesOrNo;
	scanf("%c", &yesOrNo);
}

std::string CDDA_FileFormat::GetVal(void* seekPos)
{
	std::string val = "";



	return val;
}