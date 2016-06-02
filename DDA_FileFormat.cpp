#include "DDA_FileFormat.h"

void CDDA_FileFormat::ReadXMLDefinitionFile()
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("DDA_Definition.xml");

	std::cout << "Load result: " << result.description() << " - DDA Version : " << doc.child("DDA_Definition").attribute("version").value() << std::endl;

	SDDADefinition def;
	def.DDAversion = doc.child("DDA_Definition").attribute("version").as_int();
	def.frequency = doc.child("DDA_Definition").attribute("frequency").as_int();
	def.clock = doc.child("DDA_Definition").attribute("clock").as_float();
	def.headerSize = doc.child("DDA_Definition").attribute("headersize").as_int();

	pugi::xml_node paramListNode = doc.child("DDA_Definition").child("EntryToRead");
	for (pugi::xml_node param = paramListNode.child("Param"); param; param = param.next_sibling("Param"))
	{
		SDDAParam pa;
        
		int size = strlen(param.attribute("name").value());//doesn't work...
		pa.name = new char[size];
		memcpy(pa.name, param.attribute("name").value(), size);

		pa.bitsize = param.attribute("bitsize").as_int();
		pa.interval = param.attribute("interval").as_float();

		def.inputParameters.push_back(pa);
	}

	m_definition = def;
}

int CDDA_FileFormat::InitDefinition()
{
	pugi::xml_document doc;
	if (doc.load_file("DDA_Definition.xml"))
	{
		ReadXMLDefinitionFile();
		return 1;
	}
	else
	{
		printf_s("ERROR while trying to open DDA_Definition.xml");
		return -1;
	}
}

void CDDA_FileFormat::PrintDefinition()
{
	std::cout << "DDA file base info: version=" << m_definition.DDAversion << " clock=" << m_definition.clock << " headerSize=" << m_definition.headerSize << std::endl;
	std::cout << "Parameter List:" << std::endl;

	std::vector<SDDAParam>::iterator it = m_definition.inputParameters.begin();
	while (it != m_definition.inputParameters.end())
	{
		std::cout << "Param: name=" << (it)->name << " bitsize=" << (it)->bitsize << " interval=" << (it)->interval << std::endl;
		it++;
	}
}